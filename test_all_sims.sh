for file in archive/data/*; do
    for config in archive/configs/*; do
        echo "[PROGRESS]" $file $config
        ./cmake-build-release/bin/simmain "$file" "$config";
    done
done
