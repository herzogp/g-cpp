let
  pkgs = import <nixpkgs> {};
in
  with pkgs;
  stdenv.mkDerivation {
    pname = "anthill";
    version = "0.0.1";

    src = ./.;

    buildInputs = [
      cmake
    ];

    configurePhase = ''
      cmake -DBUILD_SHARED_LIBS=NO .
    '';

    # patchPhase = '' '';

    buildPhase = ''
      #env
      make anthill
      #ls -lR 
    '';

    # checkPhase = '' '';

    installPhase = ''
      mkdir -p $out/lib
      cp libanthill.a $out/lib/libanthill.a
      mkdir -p $out/include
      cp -pra include/* $out/include/
    '';

  }
