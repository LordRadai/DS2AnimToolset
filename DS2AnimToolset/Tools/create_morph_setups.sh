mkdir output

for folder in input/*/; do
	if [ -d "$folder" ]; then
		echo "Processing $folder"
		dir=$(basename "$folder")
		
		output_folder=output/"$dir"
		mkdir -p "$output_folder"/XMD/model_xmd
		mkdir -p "$output_folder"/XMD/motion_xmd
		mkdir -p "$output_folder"/morphemeMarkup/model_xmd
		mkdir -p "$output_folder"/morphemeMarkup/motion_xmd
		
		scripts/create_morph_setup.sh "$folder" "$output_folder"
	fi
done