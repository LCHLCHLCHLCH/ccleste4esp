import tkinter as tk
from tkinter import filedialog
from PIL import Image, ImageTk

class ColorPickerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("取色小软件")
        
        self.scale = 2.0  # 初始化缩放比例
        
        self.canvas = tk.Canvas(root, width=600, height=400)
        self.canvas.pack()
        
        self.color_display = tk.Label(root, text=" ", bg="white", width=20, height=2)
        self.color_display.pack()
        
        self.label = tk.Label(root, text="RGB: ")
        self.label.pack()
        
        self.button = tk.Button(root, text="导入图片", command=self.load_image)
        self.button.pack()
        
        self.image = None
        self.photo_image = None
        
        self.canvas.bind("<Button-1>", self.get_color)
        self.canvas.bind("<MouseWheel>", self.zoom)  # 绑定鼠标滚轮事件
    
    def load_image(self):
        file_path = filedialog.askopenfilename()
        if file_path:
            self.image = Image.open(file_path)
            self.show_image()
    
    def show_image(self):
        if self.image:
            resized_image = self.image.resize(
                (int(self.image.width * self.scale), int(self.image.height * self.scale)), Image.Resampling.LANCZOS)
            self.photo_image = ImageTk.PhotoImage(resized_image)
            self.canvas.create_image(0, 0, anchor=tk.NW, image=self.photo_image)
            self.canvas.config(scrollregion=self.canvas.bbox(tk.ALL))
    
    def get_color(self, event):
        if self.image:
            x, y = int(event.x / self.scale), int(event.y / self.scale)
            if 0 <= x < self.image.width and 0 <= y < self.image.height:
                rgb = self.image.getpixel((x, y))
                self.label.config(text=f"RGB: {rgb}  {hex(((rgb[0] & 0xFF) << 16) | ((rgb[1] & 0xFF) << 8) | (rgb[2] & 0xFF))}")
                self.color_display.config(bg=f'#{rgb[0]:02x}{rgb[1]:02x}{rgb[2]:02x}')
    
    def zoom(self, event):
        if event.delta > 0:
            self.scale *= 1.1
        elif event.delta < 0:
            self.scale /= 1.1
        self.show_image()

if __name__ == "__main__":
    root = tk.Tk()
    app = ColorPickerApp(root)
    root.mainloop()
