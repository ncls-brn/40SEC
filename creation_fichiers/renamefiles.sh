renamefiles() {
    local from=$1
    local to=$2    

    for i in *"$from"; do
        if [[ -f "$i" ]]; then
            mv "$i" "${i%$from}$to"  
            echo "Renommé : $i -> ${i%$from}$to"  
        fi
    done
}

read -p "Entrez l'extension de fichiers à renommer (ex: .txt) : " ext_from
read -p "Entrez la nouvelle extension (ex: .jpg) : " ext_to

renamefiles "$ext_from" "$ext_to"

echo "Changement d'extension terminé !"

