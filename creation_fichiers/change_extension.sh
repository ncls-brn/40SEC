for file in *.txt
do
    if [[ -f "$file" ]]; then
        mv "$file" "${file%.txt}.jpg"  
        echo "Renommé : $file -> ${file%.txt}.jpg"
    fi
done

echo "Changement d'extension terminé !"

