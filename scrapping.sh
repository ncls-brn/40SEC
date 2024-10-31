#!/bin/bash

# URL du sitemap
url="https://readi.fi/sitemap.xml"

# Télécharger le contenu du sitemap
pages_return=$(curl -s "$url")

# Extraire les liens qui commencent par https://readi.fi/asset et les stocker dans un fichier
echo "$pages_return" | grep -o 'https://readi\.fi/asset[^"<]*' | sort -u > urlscrap.txt

# Compter et afficher le nombre de lignes (liens) dans urlscrap.txt
line_count=$(wc -l < urlscrap.txt)
echo "Nombre de liens trouvés : $line_count"

# Fonction pour insérer des données dans une base de données SQLite
function insert_db() {
    # Ajouter des guillemets autour des arguments pour les passer correctement à la requête SQL
    sqlite3 scrapping_data.db <<EOF
INSERT INTO TITLE_NAME
 (URL, title, description) VALUES ('$1', '$2', '$3');
EOF
}

# Fonction de scraping pour extraire le titre et la description
function scraping() {
    # Requête GET pour récupérer le contenu de la page
    pages_return=$(curl -s "$1")
    
    # Extraire le contenu de la balise <title>
    title=$(echo "$pages_return" | grep -oP '(?<=<title>).*?(?=</title>)')
    
    # Afficher le titre (pour vérification)
    echo "Title: $title"
    
    # Extraire le contenu de la balise <meta name="description">
    description=$(echo "$pages_return" | grep -oP '(?<=<meta name="description" content=").*?(?=")')
    
    # Afficher la description (pour vérification)
    echo "Description: $description"
    
    # Insérer les données extraites dans la base de données
    insert_db "$1" "$title" "$description"
}

# Lire chaque URL dans urlscrap.txt et appliquer la fonction de scraping
while IFS= read -r page_url; do
    scraping "$page_url"
done < urlscrap.txt
