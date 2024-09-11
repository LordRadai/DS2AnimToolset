input_folder="$1"
output_folder="$2"

if	[ -d "$input_folder"/morphemeMarkup ]; then
	for	xml_markup in "$input_folder"/morphemeMarkup/*.xml; do
		echo "Copying "$xml_markup" to "$output_folder"/morphemeMarkup/motion_xmd"
		cp "$xml_markup" "$output_folder"/morphemeMarkup/motion_xmd
	done
else
	echo "Could not find morphemeMarkup folder"
fi

if	[ -d "$input_folder"/motion_xmd ]; then
	for	xmd_anim in "$input_folder"/motion_xmd/*.xmd; do
		echo "Copying "$xmd_anim" to "$output_folder"/XMD/motion_xmd"
		cp "$xmd_anim" "$output_folder"/XMD/motion_xmd
	done
else
	echo "Could not find motion_xmd folder"
fi

if	[ -f "$input_folder"/*.xmd ]; then
	echo "Copying "$input_folder"/*.xmd to "$output_folder"/XMD/Rigs"
	cp "$input_folder"/*.xmd "$output_folder"/XMD/Rigs
else
	echo "Could not find .xmd model file"
fi