# make a prediction for a new image.
from PIL import Image
import cv2
import matplotlib.pyplot as plt

filename = 'input.dds'

file = Image.open(filename)

limit = (
    file.size[0] // 20,
    file.size[1] // 20
)
print(limit)

def rect_check(rect):
    if rect[2] > limit[0] and rect[3] > limit[1]:
        return True
    return False

file.save('temp.png', 'PNG')


image = cv2.imread('temp.png')
image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
image = cv2.threshold(image, 1, 255, cv2.THRESH_BINARY)[1]
cv2.imwrite('preprocessed0.png', image)
image = cv2.dilate(image, None, iterations=1)
cv2.imwrite('preprocessed1.png', image)
contours, _ = cv2.findContours(image.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

contours = [i for i in contours if rect_check(cv2.boundingRect(i))]

contours_image = cv2.drawContours(cv2.imread('temp.png'), contours, -1, (255,0,0), 5);

font = cv2.FONT_HERSHEY_SIMPLEX

results_txt = open('results.txt', 'w')

for i, c in enumerate(contours):
    rect = cv2.boundingRect(c)
    x,y,w,h = rect
    if not rect_check(rect):
        continue
    cv2.rectangle(image, (x,y), (x+w, y+h), color=(0, 255, 0), thickness=2)
    
    texture = image[y:y+h, x:x+w]
    print(f"Texture #{i}: ")
    print(x,y)
    print(x+w,y+h)
    print("size: ", w, h)
    print("-----------------")

    results_txt.write(f"Texture #{i}: \r {x} {y} {x+w} {y+h} \r size: {w} {h} \r ----------------- \r")

    cv2.imwrite(f'texture{i}.png', texture)    

    cv2.rectangle(contours_image, (x,y), (x+w, y+h), color=(0, 255, 0), thickness=4)
    cv2.putText(contours_image, f"Texture #{i}. Rect: {{{x}, {y}, {x+w}, {y+h}}}", (x+5,y+30), font, 1, (0, 255, 0), 2, cv2.LINE_AA)
cv2.imwrite('countours.png', contours_image)
results_txt.close()