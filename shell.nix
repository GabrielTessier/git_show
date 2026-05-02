{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    SDL2
    SDL2_ttf
    sdl3
    pkg-config
  ];
  shellHook = ''
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${pkgs.lib.makeLibraryPath [
    pkgs.SDL2
    pkgs.SDL2_ttf
    pkgs.sdl3
  ]};
  '';
}
