#!/bin/bash

echo "Gestion des droits et permissions (chmod)"
echo "Configurer les permissions d'un fichier ou d'un répertoire."

read -p "Entrez le chemin du fichier ou du répertoire : " cible

if [ ! -e "$cible" ]; then
    echo "Le fichier ou répertoire '$cible' n'existe pas."
    exit 1
fi

echo "Permissions pour le propriétaire (user) :"
read -p "L'utilisateur peut-il lire le fichier ? (y/n) : " read_user
read -p "L'utilisateur peut-il écrire dans le fichier ? (y/n) : " write_user
read -p "L'utilisateur peut-il exécuter le fichier ? (y/n) : " execute_user

echo "Permissions pour le groupe (group) :"
read -p "Le groupe peut-il lire le fichier ? (y/n) : " read_group
read -p "Le groupe peut-il écrire dans le fichier ? (y/n) : " write_group
read -p "Le groupe peut-il exécuter le fichier ? (y/n) : " execute_group

echo "Permissions pour les autres (others):"
read -p "Les autres peuvent-ils lire le fichier ? (y/n) : " read_other
read -p "Les autres peuvent-ils écrire dans le fichier ? (y/n) : " write_other
read -p "Les autres peuvent-ils exécuter le fichier ? (y/n) : " execute_other

convert_permission() {
    local read_perm=$1
    local write_perm=$2
    local execute_perm=$3
    local result=0

    [[ "$read_perm" == "y" ]] && result=$((result + 4))
    [[ "$write_perm" == "y" ]] && result=$((result + 2))
    [[ "$execute_perm" == "y" ]] && result=$((result + 1))

    echo $result
}

user_perm=$(convert_permission $read_user $write_user $execute_user)
group_perm=$(convert_permission $read_group $write_group $execute_group)
other_perm=$(convert_permission $read_other $write_other $execute_other)

chmod "${user_perm}${group_perm}${other_perm}" "$cible"
echo "Les permissions ont été mises à jour avec succès : ${user_perm}${group_perm}${other_perm} pour $cible"

expliquer_permissions() {
    local perm=$1
    case $perm in
        7) echo "lecture, écriture et exécution" ;;
        6) echo "lecture et écriture" ;;
        5) echo "lecture et exécution" ;;
        4) echo "lecture uniquement" ;;
        3) echo "écriture et exécution" ;;
        2) echo "écriture uniquement" ;;
        1) echo "exécution uniquement" ;;
        0) echo "aucun droit" ;;
    esac
}

echo "Explications des permissions :"
echo "User : $(expliquer_permissions $user_perm)"
echo "Group : $(expliquer_permissions $group_perm)"
echo "Others : $(expliquer_permissions $other_perm)"
