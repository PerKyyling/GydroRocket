import serial
import time
import arcade
import numpy as np
import math

WIDTH, HEIGHT = 800, 600
CENTER_X, CENTER_Y = WIDTH // 2, HEIGHT // 2
VECTOR_LENGTH = 150

class Simulation(arcade.Window):
    def __init__(self, width, height, title):
        super().__init__(width, height, title, resizable=True)
        self.background_color = arcade.color.TEA_GREEN
        self.arduino = serial.Serial(port='COM5', baudrate=9600, timeout=1)
        self.text = arcade.Text("X: 0, Y: 0", 10, 10, arcade.color.RED, 20)
        self.x = 0
        self.y = -VECTOR_LENGTH
        self.z = 0
        self.totalX = 0
        self.totalY = 0
        self.totalZ = 0

    def rotate_vector(self, rotate_x, rotate_z):
        x, y, z = self.x, self.y, self.z

        # 1. Сначала поворот вокруг X
        angle_x_rad = math.radians(rotate_x)
        cos_x = math.cos(angle_x_rad)
        sin_x = math.sin(angle_x_rad)

        y1 = y * cos_x - z * sin_x
        z1 = y * sin_x + z * cos_x
        x1 = x

        # 2. Потом поворот вокруг Z
        angle_z_rad = math.radians(rotate_z)
        cos_z = math.cos(angle_z_rad)
        sin_z = math.sin(angle_z_rad)

        x2 = x1 * cos_z - y1 * sin_z
        y2 = x1 * sin_z + y1 * cos_z
        z2 = z1

        self.totalX, self.totalY, self.totalZ = x2, y2, z2

    def from3Dto2D(self, x, y, z):
        focal_length = 300
        camera_z = 300

        z_rel = z + camera_z

        if z_rel <= 0.1:
            return (CENTER_X, CENTER_Y)

        x_proj = x / z_rel * focal_length
        y_proj = y / z_rel * focal_length

        return (CENTER_X + int(x_proj), CENTER_Y - int(y_proj))

    def on_update(self, delta_time: float):
        try:
            if self.arduino.in_waiting > 0:
                raw_data = self.arduino.readline()
                data_string = raw_data.decode('utf-8').rstrip().split()
                if len(data_string) == 4:
                    z, x = float(data_string[1]), float(data_string[3])
                    self.rotate_vector(x, -z)
                    self.text.value = f"Z: {z}, X: {x}"

        except Exception as e:
            print(f"Error: {str(e)}")
            self.arduino.close()

    def on_draw(self):
        self.clear()
        self.text.draw()
        origin = (CENTER_X, CENTER_Y)

        end_2d = self.from3Dto2D(self.totalX, self.totalY, self.totalZ)
        arcade.draw_line(origin[0], origin[1], end_2d[0], end_2d[1],
                         arcade.color.RED, 5)

def main():
    sm = Simulation(800, 600, "simulation")
    arcade.run()


if __name__ == "__main__":
    print("enabling simulation")
    main()