from PyQt5.QtWidgets import QGraphicsTextItem
from PyQt5.QtGui import QColor

class QGraphicsTextItemWithBackground(QGraphicsTextItem):
    def __init__(self,text,background = QColor(255,255,255,255),border = QColor(0,0,0,255)):
        super().__init__(text)
        self.background = background
        self.border = border

    def paint(self,painter, o, w):
        painter.setBrush(self.background);
        painter.setPen(self.border);
        painter.drawRect(self.boundingRect());
        super().paint(painter,o, w);
