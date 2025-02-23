#!/usr/bin/env /bin/bash

HASH_SIZE=6
DIR=$(git rev-parse --show-toplevel 2>&1)
ERROR=$(echo $DIR | grep fatal)

if [ ! -z "$ERROR" ]; then
    echo "Not a git repo"
    exit
fi

GIT_DIR="$DIR/.git"
OBJ="$GIT_DIR/objects/*"

ALL_HASH=$(git rev-list --objects --all)

function get_name()
{
    if [ ! -z "$1" ]; then
        echo $(basename $1 | sed "s/_/\\\_/g" | sed "s/#/\\\#/g")
    fi
}

function cmp_deb()
{
    STR=""
    for i in `seq $1`
    do
        STR="| $STR"
    done
    echo "$STR "
}

function print_blob()
{
    #DEBUT=$(cmp_deb $2)
    full_name=$(echo -e "$ALL_HASH" | grep $1 | cut -d' ' -f2)
    #echo $DEBUT"BLOB : $(echo $1 | head -c$HASH_SIZE) ($(get_name $full_name))"
    #echo "[\text{$(echo $1 | head -c$HASH_SIZE) ($(get_name $full_name))}]"
    echo "[$(get_name $full_name) ($(echo $1 | head -c$HASH_SIZE))]"
}

function print_tree()
{
    #DEBUT=$(cmp_deb $2)
    full_name=$(echo -e "$ALL_HASH" | grep $1 | cut -d' ' -f2)
    #echo $DEBUT"TREE : $(echo $1 | head -c$HASH_SIZE) ($(get_name $full_name))"
    #echo "[\text{$(echo $1 | head -c$HASH_SIZE) ($(get_name $full_name))}"
    echo "[$(get_name $full_name) ($(echo $1 | head -c$HASH_SIZE))"
    # content=$(git cat-file -p $1)
    #PR_content=$(git cat-file -p $1 | sed "s/^/$DEBUT/g")
    #echo -e "$PR_content"

    H=$(git cat-file -p $1 | cut -d' ' -f3 | cut -f1)
    for sub in $H
    do
        type=$(git cat-file -t $sub)
        #echo $type
        if [ "$type" == "blob" ]; then
            print_blob $sub $(($2+1))
        elif [ "$type" == "tree" ]; then
            print_tree $sub $(($2+1))
        else
            echo "$DEBUT $type not suported" 1>&2
        fi
    done

    echo "]"
}

function print_commit()
{
    #DEBUT=$(cmp_deb $2)
    #echo $DEBUT"COMMIT : $(echo $1 | head -c$HASH_SIZE)"
    content=$(git cat-file -p $1 | sed "s/^/$DEBUT/g")
    #echo -e "$content"
    #echo $DEBUT
    tree=$(echo "$content" | grep "tree" | sed -s "s/^.* //")
    #echo "[\text{$(echo $1 | head -c$HASH_SIZE)}"
    echo "[COMMIT $(echo $1 | head -c$HASH_SIZE)"
    print_tree $tree $(($2+1))
    echo "]"

    #echo -e "\n\n"
    #parent=$(echo "$content" | grep "parent" | sed -s "s/^.* //")
    #print_commit $parent $2
}


commit=$(git rev-parse HEAD)


echo "\documentclass[tikz,border=10pt]{standalone}
\usepackage{forest}
\begin{document}
\begin{forest}" > graph.tex

print_commit $commit 1 >> graph.tex

echo "\end{forest}
\end{document}" >> graph.tex

pdflatex graph.tex
