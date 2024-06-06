import serial
import pathlib
import cv2

# Configurarea portului serial pentru Arduino Uno
ser = serial.Serial('COM12', 9600)  # Import si utilizare clasa Serial

# Definirea caii catre fisierul cascade
cascade_path = pathlib.Path(cv2.__file__).parent.absolute() / "data/haarcascade_frontalface_default.xml"

# Incarcarea clasificatorului cascade
clf = cv2.CascadeClassifier(str(cascade_path))

# Initializarea capturarii video
camera = cv2.VideoCapture(2) # (2) pentru camera web folosita in proiect (0) pentru camera default web a laptopului

while True:
    # Captarea unui cadru
    _, frame = camera.read()

    # Conversia cadrului in grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detectarea fetelor din cadru grayscale
    faces = clf.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30),
        flags=cv2.CASCADE_SCALE_IMAGE
    )

    # Verificarea detectarii unei fete si procesarea acesteia
    if len(faces) > 0:
        # Alegerea primei fete detectate
        x, y, w, h = faces[0]

        # Desenarea unui dreptunghi in jurul fetei
        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

        # Calculul centrului fetei
        face_center_x = x + w // 2
        face_center_y = y + h // 2

        # Trimiterea coordonatelor X si Y ale fetei catre Arduino prin portul serial
        ser.write(f"{face_center_x},{face_center_y}\n".encode('utf-8'))

    # Afisarea cadrului
    cv2.imshow("Faces", frame)

    # Verificarea apasarii tastei "q" pentru iesire din program
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Eliberarea capturarii video
camera.release()

# Inchiderea tuturor ferestrelor
cv2.destroyAllWindows()
