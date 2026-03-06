for dataset_file in archive/distributions_memory/datasets/*.csv; do
    filename=$(basename "$dataset_file" .csv)
    config_file="archive/distributions_memory/configs/${filename}.json"
    if [[ -f "$config_file" ]]; then
        ./cmake-build-release/bin/distmain "$dataset_file" "$config_file"
    else
        echo "Config file not found for $dataset_file"
    fi
done
