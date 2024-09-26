source "config.txt"
tmp_folder="tmp"

if [ -d "$witchybnd" ]; then
	echo "Specified WitchyBND folder does not exist ($witchybnd)"
	exit 1
fi

if [ -z "$(find "input/" -maxdepth 1 -type f)" ]; then
	echo "There is no ccm file in input"
	exit 1
fi

file=$(find input/ -type f -print -quit)

cp "$file" "$tmp_folder/font.ccm"

mkdir "$tmp_folder/font-fontbnd-dcx"

echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<bnd4>
  <filename>font.fontbnd.dcx</filename>
  <compression>DCX_DFLT_10000_24_9</compression>
  <version>14M18O9</version>
  <format>IDs, Names2, Compression</format>
  <bigendian>False</bigendian>
  <bitbigendian>False</bitbigendian>
  <unicode>False</unicode>
  <extended>0x00</extended>
  <unk04>False</unk04>
  <unk05>False</unk05>
  <files>" > "$tmp_folder/font-fontbnd-dcx/_witchy-bnd4.xml"
  
echo "	<file>
      <flags>Flag1</flags>
      <id>0</id>
      <path>font.ccm</path>
    </file>" >> "$tmp_folder/font-fontbnd-dcx/_witchy-bnd4.xml"
	
echo "	<file>
      <flags>Flag1</flags>
      <id>1</id>
      <path>font.tpf</path>
    </file>" >> "$tmp_folder/font-fontbnd-dcx/_witchy-bnd4.xml"
	
echo "  </files>
</bnd4>" >> "$tmp_folder/font-fontbnd-dcx/_witchy-bnd4.xml"

cp "$tmp_folder/font.ccm" "$tmp_folder/font-fontbnd-dcx"/
cp "$tmp_folder/font.tpf" "$tmp_folder/font-fontbnd-dcx"/

"$witchybnd" "$tmp_folder/font-fontbnd-dcx"

cp "$tmp_folder/font.fontbnd.dcx" "font.fontbnd.dcx"