
if [ -z "$1" ]; then
    read -p "Entrez le message de commit : " commit_message
else
    commit_message="$1"
fi

git add .
git commit -m "$commit_message"
git push

echo "Les changements ont été poussés avec succès !"

