{
  description = "C++ DevEnv template";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell rec {
          name = "C++";

          packages = with pkgs; [
            # Dev tools
            cmake
            cmakeCurses

            # Build time dependencies

            # Runtime dependencies
          ];

          shellHook = ''
            export PS1="Nix! DevEnv: ${name} > "
          '';
        };

        defaultPackage = pkgs.callPackage ./default.nix {};
      }
    );
}
