let
  pkgs = import <nixpkgs> {};
in
  with pkgs;
  stdenv.mkDerivation {
    pname = "glitch-grid-cpp";
    version = "0.0.3";

    src = ./.;

    buildInputs = [
      cmake
    ];

    configurePhase = ''
      cmake .
    '';

    # patchPhase = '' '';

    buildPhase = ''
      #env
      make control
      #ls -lR 
    '';

    # checkPhase = '' '';

    installPhase = ''
      mkdir -p $out/bin
      cp control $out/bin/control
    '';

  }

