import cv2
import urllib.request
import numpy as np

url = 'http://192.168.0.188'
# '''cam.bmp / cam-lo.jpg / cam-hi.jpg / cam.mjpeg '''
cv2.namedWindow("live transmission", cv2.WINDOW_AUTOSIZE)

while True:
    img_resp = urllib.request.urlopen(url + 'cam-hi.jpg')
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    img = cv2.imdecode(imgnp, -1)
    
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    canny = cv2.Canny(cv2.GaussianBlur(gray, (11, 11), 0), 30, 150, 3)
    dilated = cv2.dilate(canny, (1, 1), iterations=2)
    (Cnt, _) = cv2.findContours(dilated.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    
    k = img.copy()  # Ensure we don't modify the original image
    cv2.drawContours(k, Cnt, -1, (0, 255, 0), 2)
    
    # Count the number of contours and put text on the image
    cow = len(Cnt)
    cv2.putText(k, f'Object Count: {cow}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
    
    cv2.imshow("mit contour", canny)
    cv2.imshow("live transmission", k)  # Display image with object count

    key = cv2.waitKey(5)
    
    if key == ord('q'):
        break
    elif key == ord('a'):
        print(cow)

cv2.destroyAllWindows()
