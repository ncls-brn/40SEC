url="https://readi.fi/sitemap.xml" 

pages_return=$(curl -s "$url")

links_of_pages=$(echo "$pages_return" | grep -o 'https://readi\.fi\/asset[^"<]*'> urlscrap.txt)

line_count=$(wc -l < urlscrap.txt)
echo "Nombre de liens trouvés : $line_count"

