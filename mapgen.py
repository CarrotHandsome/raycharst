import tkinter as tk
import tkinter.messagebox as messagebox
from tkinter import ttk
from collections import deque

class MapDrawer:
    def __init__(self, root):
        self.root = root
        self.root.title("Map Drawing Program")
        self.root.geometry("800x600")
        
        self.rows = 10
        self.cols = 10
        self.cell_size = 30
        self.grid_data = [[0 for _ in range(self.cols)] for _ in range(self.rows)]
        self.initialize_perimeter()  # Set the perimeter to walls
        
        # Variables for rectangle drawing
        self.start_cell = None
        self.current_cell = None
        self.drawing_rectangle = False
        self.preview_rectangles = []
        
        self.current_tool = "draw"  # Current selected tool: "draw", "rect", "fill"
        
        # Main frame
        self.main_frame = ttk.Frame(root, padding="10")
        self.main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Control panel
        self.control_frame = ttk.Frame(self.main_frame, padding="5")
        self.control_frame.pack(side=tk.TOP, fill=tk.X)
        
        # Grid dimensions controls
        ttk.Label(self.control_frame, text="Rows:").pack(side=tk.LEFT, padx=5)
        self.rows_var = tk.StringVar(value=str(self.rows))
        self.rows_entry = ttk.Entry(self.control_frame, textvariable=self.rows_var, width=5)
        self.rows_entry.pack(side=tk.LEFT, padx=5)
        
        ttk.Label(self.control_frame, text="Columns:").pack(side=tk.LEFT, padx=5)
        self.cols_var = tk.StringVar(value=str(self.cols))
        self.cols_entry = ttk.Entry(self.control_frame, textvariable=self.cols_var, width=5)
        self.cols_entry.pack(side=tk.LEFT, padx=5)
        
        ttk.Button(self.control_frame, text="Update Grid", command=self.update_grid).pack(side=tk.LEFT, padx=10)
        ttk.Button(self.control_frame, text="Clear Grid", command=self.clear_grid).pack(side=tk.LEFT, padx=5)
        
        # Tools frame
        self.tools_frame = ttk.LabelFrame(self.control_frame, text="Tools")
        self.tools_frame.pack(side=tk.LEFT, padx=10)
        
        self.tool_var = tk.StringVar(value="draw")
        ttk.Radiobutton(self.tools_frame, text="Draw", variable=self.tool_var, value="draw", command=self.select_tool).pack(side=tk.LEFT, padx=5)
        ttk.Radiobutton(self.tools_frame, text="Rectangle", variable=self.tool_var, value="rect", command=self.select_tool).pack(side=tk.LEFT, padx=5)
        ttk.Radiobutton(self.tools_frame, text="Fill", variable=self.tool_var, value="fill", command=self.select_tool).pack(side=tk.LEFT, padx=5)
        
        ttk.Button(self.control_frame, text="Generate C Array", command=self.generate_c_array).pack(side=tk.RIGHT, padx=5)
        
        # Canvas for drawing the grid
        self.canvas_frame = ttk.Frame(self.main_frame, padding="5")
        self.canvas_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True)
        
        self.canvas = tk.Canvas(self.canvas_frame, bg="white")
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.canvas.bind("<Configure>", self.draw_grid)
        self.canvas.bind("<Button-1>", self.handle_mouse_click)
        self.canvas.bind("<B1-Motion>", self.handle_mouse_drag)
        self.canvas.bind("<ButtonRelease-1>", self.handle_mouse_release)
        
        # Output frame
        self.output_frame = ttk.Frame(self.main_frame, padding="5")
        self.output_frame.pack(side=tk.BOTTOM, fill=tk.X)
        
        self.output_text = tk.Text(self.output_frame, height=10, wrap=tk.WORD)
        self.output_text.pack(fill=tk.X)
        
        copy_button = ttk.Button(self.output_frame, text="Copy to Clipboard", command=self.copy_to_clipboard)
        copy_button.pack(side=tk.RIGHT, padx=5, pady=5)
        
        # Initial grid drawing
        self.draw_grid()
    
    def initialize_perimeter(self):
        """Set the perimeter of the grid to walls (1)"""
        for col in range(self.cols):
            self.grid_data[0][col] = 1  # Top row
            self.grid_data[self.rows-1][col] = 1  # Bottom row
        
        for row in range(self.rows):
            self.grid_data[row][0] = 1  # Left column
            self.grid_data[row][self.cols-1] = 1  # Right column
    
    def draw_grid(self, event=None):
        self.canvas.delete("all")
        
        # Calculate available space
        canvas_width = self.canvas.winfo_width()
        canvas_height = self.canvas.winfo_height()
        
        # Calculate cell size based on available space
        self.cell_size = min(canvas_width // self.cols, canvas_height // self.rows)
        
        # Center the grid
        x_offset = (canvas_width - self.cell_size * self.cols) // 2
        y_offset = (canvas_height - self.cell_size * self.rows) // 2
        
        # Draw cells
        for row in range(self.rows):
            for col in range(self.cols):
                x1 = x_offset + col * self.cell_size
                y1 = y_offset + row * self.cell_size
                x2 = x1 + self.cell_size
                y2 = y1 + self.cell_size
                
                # Determine fill color based on cell value
                fill_color = "black" if self.grid_data[row][col] == 1 else "white"
                
                # Create rectangle with border
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=fill_color, outline="gray", tags=f"cell_{row}_{col}")
        
        # Redraw rectangle preview if needed
        if self.drawing_rectangle and self.start_cell and self.current_cell:
            self.draw_rectangle_preview()
    
    def get_cell_at_position(self, x, y):
        """Convert screen coordinates to grid cell coordinates"""
        canvas_width = self.canvas.winfo_width()
        canvas_height = self.canvas.winfo_height()
        
        # Calculate offsets
        x_offset = (canvas_width - self.cell_size * self.cols) // 2
        y_offset = (canvas_height - self.cell_size * self.rows) // 2
        
        # Convert click coordinates to grid cell
        col = (x - x_offset) // self.cell_size
        row = (y - y_offset) // self.cell_size
        
        # Make sure the click is within the grid
        if 0 <= row < self.rows and 0 <= col < self.cols:
            return row, col
        return None
    
    def handle_mouse_click(self, event):
        cell = self.get_cell_at_position(event.x, event.y)
        if not cell:
            return
        
        row, col = cell
        
        if self.tool_var.get() == "draw":
            # Toggle cell value for draw tool
            self.grid_data[row][col] = 1 - self.grid_data[row][col]
            self.draw_grid()
        elif self.tool_var.get() == "rect":
            # Start rectangle
            self.start_cell = cell
            self.current_cell = cell
            self.drawing_rectangle = True
        elif self.tool_var.get() == "fill":
            # Fill connected area
            self.fill_area(row, col)
            self.draw_grid()
    
    def handle_mouse_drag(self, event):
        cell = self.get_cell_at_position(event.x, event.y)
        if not cell:
            return
        
        if self.tool_var.get() == "rect" and self.drawing_rectangle:
            self.current_cell = cell
            self.draw_rectangle_preview()
    
    def handle_mouse_release(self, event):
        if self.tool_var.get() == "rect" and self.drawing_rectangle:
            if self.start_cell and self.current_cell:
                self.apply_rectangle()
            
            # Clear rectangle drawing state
            self.drawing_rectangle = False
            self.start_cell = None
            self.current_cell = None
            self.clear_rectangle_preview()
            self.draw_grid()
    
    def draw_rectangle_preview(self):
        """Draw a preview of the rectangle during drag"""
        self.clear_rectangle_preview()
        
        if not self.start_cell or not self.current_cell:
            return
        
        # Get corners of the rectangle
        start_row, start_col = self.start_cell
        end_row, end_col = self.current_cell
        
        # Ensure start is top-left, end is bottom-right
        min_row = min(start_row, end_row)
        max_row = max(start_row, end_row)
        min_col = min(start_col, end_col)
        max_col = max(start_col, end_col)
        
        # Calculate available space
        canvas_width = self.canvas.winfo_width()
        canvas_height = self.canvas.winfo_height()
        
        # Center the grid
        x_offset = (canvas_width - self.cell_size * self.cols) // 2
        y_offset = (canvas_height - self.cell_size * self.rows) // 2
        
        # Draw preview for each cell in the rectangle
        for row in range(min_row, max_row + 1):
            for col in range(min_col, max_col + 1):
                # Only draw preview for cells that would change
                if self.grid_data[row][col] != 1:
                    x1 = x_offset + col * self.cell_size
                    y1 = y_offset + row * self.cell_size
                    x2 = x1 + self.cell_size
                    y2 = y1 + self.cell_size
                    
                    # Create rectangle with semi-transparent fill
                    rect_id = self.canvas.create_rectangle(x1, y1, x2, y2, fill="gray", stipple="gray50", outline="blue", tags="preview")
                    self.preview_rectangles.append(rect_id)
    
    def clear_rectangle_preview(self):
        """Clear all preview rectangles"""
        self.canvas.delete("preview")
        self.preview_rectangles = []
    
    def apply_rectangle(self):
        """Apply the rectangle to the grid data"""
        if not self.start_cell or not self.current_cell:
            return
        
        # Get corners of the rectangle
        start_row, start_col = self.start_cell
        end_row, end_col = self.current_cell
        
        # Ensure start is top-left, end is bottom-right
        min_row = min(start_row, end_row)
        max_row = max(start_row, end_row)
        min_col = min(start_col, end_col)
        max_col = max(start_col, end_col)
        
        # Set cells to walls (1)
        for row in range(min_row, max_row + 1):
            for col in range(min_col, max_col + 1):
                if row == min_row or row == max_row or col == min_col or col == max_col:
                    # Only set the perimeter of the rectangle to walls
                    self.grid_data[row][col] = 1
    
    def select_tool(self):
        self.current_tool = self.tool_var.get()
        # Clear any ongoing operations
        self.drawing_rectangle = False
        self.start_cell = None
        self.current_cell = None
        self.clear_rectangle_preview()
    
    def fill_area(self, start_row, start_col):
        """Fill a connected area with walls using BFS flood fill algorithm"""
        target_value = self.grid_data[start_row][start_col]
        new_value = 1 - target_value  # Invert the value
        
        # If the start cell is already the desired value, do nothing
        if target_value == new_value:
            return
        
        # BFS to fill connected cells
        queue = deque([(start_row, start_col)])
        self.grid_data[start_row][start_col] = new_value
        
        while queue:
            row, col = queue.popleft()
            
            # Check adjacent cells (up, down, left, right)
            directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
            for dr, dc in directions:
                new_row, new_col = row + dr, col + dc
                
                # Check if the cell is within bounds and has the target value
                if (0 <= new_row < self.rows and 
                    0 <= new_col < self.cols and 
                    self.grid_data[new_row][new_col] == target_value):
                    self.grid_data[new_row][new_col] = new_value
                    queue.append((new_row, new_col))
    
    def update_grid(self):
        try:
            new_rows = int(self.rows_var.get())
            new_cols = int(self.cols_var.get())
            
            if new_rows <= 2 or new_cols <= 2:
                messagebox.showerror("Invalid Input", "Rows and columns must be at least 3")
                return
            
            if new_rows > 50 or new_cols > 50:
                if not messagebox.askyesno("Warning", "Large grid sizes may affect performance. Continue?"):
                    return
            
            # Create new grid data with preserved values from the old grid where possible
            new_grid_data = [[0 for _ in range(new_cols)] for _ in range(new_rows)]
            
            for row in range(min(self.rows, new_rows)):
                for col in range(min(self.cols, new_cols)):
                    new_grid_data[row][col] = self.grid_data[row][col]
            
            self.rows = new_rows
            self.cols = new_cols
            self.grid_data = new_grid_data
            
            # Ensure perimeter is walls
            self.initialize_perimeter()
            
            self.draw_grid()
            
        except ValueError:
            messagebox.showerror("Invalid Input", "Rows and columns must be integers")
    
    def clear_grid(self):
        # Clear the grid but keep the perimeter as walls
        self.grid_data = [[0 for _ in range(self.cols)] for _ in range(self.rows)]
        self.initialize_perimeter()
        self.draw_grid()
    
    def generate_c_array(self):
        # Format as C array with rows
        array_str = "int map[] = {\n"
        
        # Add each row with proper formatting
        for row in range(self.rows):
            array_str += "    "
            for col in range(self.cols):
                array_str += f"{self.grid_data[row][col]}, "
            # Remove trailing comma and space from the last element in the row
            if self.cols > 0:
                array_str = array_str.rstrip(", ")
            array_str += ",\n"
        
        # Remove trailing comma and newline
        array_str = array_str.rstrip(",\n") + "\n};"
        
        # Add array size and dimensions as comments
        array_str = f"// Grid dimensions: {self.rows} rows x {self.cols} columns\n" + \
                    f"// Total size: {self.rows * self.cols} elements\n" + array_str
        
        # Generate 1D array version as well
        flat_grid = []
        for row in range(self.rows):
            flat_grid.extend(self.grid_data[row])
        
        array_1d = "// 1D version (row-major order):\n"
        array_1d += "int map_1d[] = {\n    "
        for i, val in enumerate(flat_grid):
            array_1d += f"{val}, "
            if (i + 1) % self.cols == 0 and i < len(flat_grid) - 1:  # Line break after each row
                array_1d += "\n    "
        
        array_1d = array_1d.rstrip(", ") + "\n};"
        
        # Add both arrays to output
        full_output = array_str + "\n\n" + array_1d
        
        # Display in output text
        self.output_text.delete(1.0, tk.END)
        self.output_text.insert(tk.END, full_output)
    
    def copy_to_clipboard(self):
        text = self.output_text.get(1.0, tk.END)
        # Use tkinter clipboard instead of pyperclip
        self.root.clipboard_clear()
        self.root.clipboard_append(text)
        messagebox.showinfo("Info", "Copied to clipboard!")

if __name__ == "__main__":
    root = tk.Tk()
    app = MapDrawer(root)
    root.mainloop()