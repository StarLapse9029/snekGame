from turtle import Screen
import time
from snake import Snake
from food import Food
from scoreboard import Scoreboard

screen = Screen()
screen.setup(width=600, height=600)
screen.bgcolor("black")
screen.title("SnekGame")
screen.tracer(0)

speed = 1
snake = Snake()
food = Food()
scoreboard = Scoreboard()

screen.listen()
screen.onkey(snake.up, "Up")
screen.onkey(snake.down, "Down")
screen.onkey(snake.left, "Left")
screen.onkey(snake.right, "Right")
screen.onkey(screen.bye, "Escape")


game_is_on = True
while game_is_on:
    screen.update()
    time.sleep(0.1/speed)
    snake.move()    

    if(snake.head.distance(food) < 18):
        food.refresh()
        speed *= 1.1
        snake.extend()
        scoreboard.increase()

    if(snake.head.xcor() > 280 or snake.head.xcor() < -280):
        snake.head.goto(-(snake.head.xcor()), snake.head.ycor())
    if(snake.head.ycor() > 280 or snake.head.ycor() < -280):
        snake.head.goto(snake.head.xcor(), -(snake.head.ycor()))

    for segment in snake.segments[1:]:
        if(snake.head.distance(segment) < 10):
            game_is_on = False
            scoreboard.over()

screen.exitonclick()
