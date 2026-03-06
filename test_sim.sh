for file in archive/data_prepped/*; do
  ./cmake-build-release/bin/simmain "$file" test_config.json;
done
