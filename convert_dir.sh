for file in archive/$1/*.csv; do
    filename=$(basename "$file" .csv)
    tex_file="archive/tables/$1/${filename}.tex"
    python csv_to_latex.py "$file" "$tex_file" $2 $3
done