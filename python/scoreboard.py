from turtle import Turtle
from typing import ForwardRef
ALIGNMENT = "center"
FONT = ("Courier", 24, "normal")

class Scoreboard(Turtle):
    def __init__(self):
        super().__init__()
        self.score = 0
        self.penup()
        self.hideturtle()
        self.color("white")
        self.goto(0,260)
        self.update()


    def update(self):
        self.clear()
        self.write(f"Score: {self.score}", align=ALIGNMENT, font=FONT)
        

    def increase(self):
        self.score += 1
        self.update()

    def over(self):
        self.goto(0, 0)
        self.clear()
        self.write("GAME OVER", align=ALIGNMENT, font=("Courier", 26, "bold"))
        self.goto(0, -25)
        self.write(f"Final Score: {self.score}", align=ALIGNMENT, font=("Courier", 22, "normal"))
