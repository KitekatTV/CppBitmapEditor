{ lib, llvmPackages_11, cmake }:

llvmPackages_11.stdenv.mkDerivation rec {
  pname = "CppImageEditor";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [ cmake ];

  cmakeFlags = [

  ];

  meta = with lib; {
    homepage = "https://github.com/KitekatTV/CppBitmapEditor";
    description = ''
      A CLI tool for quickly editing BMP and GIF images. 
    '';
    license = licenses.mit;
    platforms = with platforms; linux ++ darwin;
  };
}
