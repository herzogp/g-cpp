# default.nix

let pkgs = import <nixpkgs> { }; in
{
  control = pkgs.callPackage ./vault.nix  { stdenv = pkgs.clangStdenv; };
} 
