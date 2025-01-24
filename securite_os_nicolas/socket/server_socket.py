import socket

HOST = "127.0.0.1"  # Standard loopback interface address
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)

# Création du serveur
try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))  # Lier l'adresse et le port
        s.listen()  # Écouter les connexions entrantes
        print(f"Server is listening on {HOST}:{PORT}")
        
        while True:
            conn, addr = s.accept()  # Accepter une connexion entrante
            with conn:
                print(f"Connected by {addr}")
                
                while True:
                    data = conn.recv(1024)  # Recevoir des données
                    if not data:
                        break  # Quitter si aucune donnée n'est reçue
                    conn.sendall(data)  # Renvoyer les données
except Exception as e:
    print(f"An error occurred: {e}")
