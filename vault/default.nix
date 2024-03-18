let
  pkgs = import <nixpkgs> {};
  anthill = import ../anthill;
in
  with pkgs;
  stdenv.mkDerivation {
    pname = "glitch-grid-cpp-vault";
    version = "0.0.5";

    src = ./.;

    buildInputs = [
      cmake
      anthill
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


