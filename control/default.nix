# default.nix

let pkgs = import <nixpkgs> { }; in
{
  control = pkgs.callPackage ./control.nix  { stdenv = pkgs.clangStdenv; };
} 
