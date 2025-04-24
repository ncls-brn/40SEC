#!/bin/bash


url="https://readi.fi/sitemap.xml"


pages_return=$(curl -s "$url")

echo "$pages_return" | grep -o 'https://readi\.fi/asset[^"<]*' | sort -u > urlscrap.txt

line_count=$(wc -l < urlscrap.txt)
echo "Nombre de liens trouvés : $line_count"

function insert_db() {
    sqlite3 scrapping_data.db <<EOF
INSERT INTO TITLE_NAME
 (URL, title, description) VALUES ('$1', '$2', '$3');
EOF
}

# En passant les valeurs directement dans la commande sqlite3 en concaténant les variables,
# cela ouvre de potentielles failles de sécurité (injection SQL).
# Typiquement si $3 est égal à "'); DROP TABLE TITLE_NAME; --",
# Il est préférable d'utiliser des paramètres pour éviter cela.
# "INSERT INTO TITLE_NAME (URL, title, description) VALUES (?, ?, ?);" "$1" "$2" "$3"

function scraping() {
    pages_return=$(curl -s "$1")
    
    title=$(echo "$pages_return" | grep -oP '(?<=<title>).*?(?=</title>)')
    
    echo "Title: $title"
    
    description=$(echo "$pages_return" | grep -oP '(?<=<meta name="description" content=").*?(?=")')
    
    echo "Description: $description"
    
    insert_db "$1" "$title" "$description"
}

while IFS= read -r page_url; do
    scraping "$page_url"
done < urlscrap.txt
