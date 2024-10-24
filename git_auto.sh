read -p "Entrez le message de commit:" commit_message

git add .

git commit -m"$commit_message"

git push 

echo " Les changements ont été poussés avec succès!"
