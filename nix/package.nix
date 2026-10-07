{ lib
, stdenv
, src
, cmake
, ninja
, pkg-config
, gsl
, libxml2
, libsamplerate
, ffmpeg-headless
, python3
, qt6
, libxslt
, docbook_xsl
, makeWrapper
, bash
, gawk
}:

let
  python = python3.withPackages (ps: [ ps.pyqt6 ]);
in
stdenv.mkDerivation {
  pname = "ndmanager-plugins";
  version = "3.0.0";
  inherit src;

  nativeBuildInputs = [ cmake ninja pkg-config python3.pkgs.pyqt6 libxslt makeWrapper qt6.wrapQtAppsHook ];
  buildInputs = [ gsl libxml2 libsamplerate ffmpeg-headless python qt6.qtbase bash ];

  cmakeFlags = [ "-DDOCBOOK_MANPAGES_XSL=${docbook_xsl}/share/xml/docbook-xsl/manpages/docbook.xsl" ];

  # The scripts call each other, gawk and ffmpeg at run time.
  postFixup = ''
    for f in $out/bin/*; do
      if head -c 2 "$f" | grep -q '#!'; then
        wrapProgram "$f" --prefix PATH : ${lib.makeBinPath [ gawk ffmpeg-headless ]}:$out/bin
      fi
    done
  '';

  meta = {
    description = "Processing tools and scripts for NDManager";
    homepage = "https://neurosuite.github.io";
    license = with lib.licenses; [ gpl3Plus lgpl21Plus ];
    platforms = lib.platforms.unix;
  };
}
