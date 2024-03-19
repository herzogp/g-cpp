# default.nix

let pkgs = import <nixpkgs> { }; in
{
  anthill = pkgs.callPackage ./anthill.nix  { stdenv = pkgs.clangStdenv; };
} 
