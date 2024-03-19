# control.nix

{stdenv, callPackage, pkgs}:
let
  anthill = callPackage ../anthill/anthill.nix { inherit stdenv; };
  sdk = callPackage ../../../../src/github/antithesis-sdk-cpp/antithesis-sdk-cpp.nix { inherit stdenv; };
in
  with pkgs;
  stdenv.mkDerivation {
    pname = "glitch-grid-cpp-control";
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
      clang -v
      # env
      # ls -lR 
      make control
    '';

    # checkPhase = '' '';

    installPhase = ''
      mkdir -p $out/bin
      cp control $out/bin/control
    '';

  }
