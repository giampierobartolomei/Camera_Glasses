# Installazione del pacchetto irr se non già installato
if (!requireNamespace("irr", quietly = TRUE)) {
  install.packages("irr")
}

# Caricamento della libreria
library(irr)
# Leggi i file
data1 <- read.csv("/home/giampiero/Desktop/Camera_Glass/eye_contact_test/gt.csv", header = FALSE, sep = ",")  # Specifica il separatore corretto
data2 <- read.csv("/home/giampiero/Desktop/Camera_Glass/eye_contact_test/output.csv", header = FALSE, sep = ",")

# Controlla le dimensioni
print(dim(data1))
print(dim(data2))

# Estrai i dati numerici
vector1 <- as.numeric(data1[, 1])  # Considera la prima colonna del file 1
vector2 <- as.numeric(data2[, 1])  # Considera la prima colonna del file 2

# Allineamento delle lunghezze
min_length <- min(length(vector1), length(vector2))
vector1 <- vector1[1:min_length]
vector2 <- vector2[1:min_length]

# Debug verifica lunghezze
print(length(vector1))
print(length(vector2))

# Creazione del data frame combinato
data_combined <- data.frame(Observer1 = vector1, Observer2 = vector2)

# Calcola la kappa - 0.785 da test con espcam
library(irr)
kappa_result <- kappa2(data_combined)
print(kappa_result)
