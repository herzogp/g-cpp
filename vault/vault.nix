# vault.nix

{stdenv, callPackage, pkgs}:
let
  anthill = callPackage ../anthill/anthill.nix { inherit stdenv; };
  sdk = callPackage ../../../../src/github/antithesis-sdk-cpp/antithesis-sdk-cpp.nix { inherit stdenv; };
in
  with pkgs;
  stdenv.mkDerivation {
    pname = "glitch-grid-cpp-vault";
    version = "0.0.6";

    src = ./.;

    nativeBuildInputs = [
      cmake
      clang_17
    ];

    buildInputs = [
      anthill
      sdk
    ];

    configurePhase = ''
      cmake .
    '';

    # patchPhase = '' '';

    buildPhase = ''
      #env
      make vault
      #ls -lR 
    '';

    # checkPhase = '' '';

    installPhase = ''
      mkdir -p $out/bin
      cp vault $out/bin/vault
    '';

  }


