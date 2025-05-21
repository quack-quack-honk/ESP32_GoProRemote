import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports

class GoProControl:
    def __init__(self, root):
        self.root = root
        self.root.title("GoPro Control")
        self.serial = None        # Serial frame
        serial_frame = ttk.LabelFrame(root, text="Serial Port", padding="5")
        serial_frame.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        
        # Port selection
        self.port_var = tk.StringVar()
        ttk.Label(serial_frame, text="Port:").grid(row=0, column=0, padx=5, pady=5)
        self.port_combo = ttk.Combobox(serial_frame, textvariable=self.port_var)
        self.port_combo.grid(row=0, column=1, padx=5, pady=5)
        ttk.Button(serial_frame, text="Refresh", command=self.refresh_ports).grid(row=0, column=2, padx=5, pady=5)
        self.serial_btn = ttk.Button(serial_frame, text="Connect Serial", command=self.connect_serial)
        self.serial_btn.grid(row=0, column=3, padx=5, pady=5)
        self.refresh_ports()
        
        # GoPro Connection frame
        gopro_frame = ttk.LabelFrame(root, text="GoPro Connection", padding="5")
        gopro_frame.grid(row=1, column=0, padx=5, pady=5, sticky="ew")
        
        # Connect button
        self.gopro_btn = ttk.Button(gopro_frame, text="Connect/Wake", command=lambda: self.send_command("connect"))
        self.gopro_btn.grid(row=0, column=0, padx=5, pady=5)
          # Mode frame
        mode_frame = ttk.LabelFrame(root, text="Mode", padding="5")
        mode_frame.grid(row=2, column=0, padx=5, pady=5, sticky="ew")
        
        # Mode buttons
        ttk.Button(mode_frame, text="Video Mode", command=lambda: self.send_command("video")).grid(row=0, column=0, padx=5, pady=5)
        ttk.Button(mode_frame, text="Photo Mode", command=lambda: self.send_command("photo")).grid(row=0, column=1, padx=5, pady=5)
        ttk.Button(mode_frame, text="Timelapse Mode", command=lambda: self.send_command("timelapse")).grid(row=0, column=2, padx=5, pady=5)
        
        # Control frame
        ctrl_frame = ttk.LabelFrame(root, text="Control", padding="5")
        ctrl_frame.grid(row=3, column=0, padx=5, pady=5, sticky="ew")
        
        # Control buttons
        ttk.Button(ctrl_frame, text="Trigger", command=lambda: self.send_command("trigger")).grid(row=0, column=0, padx=5, pady=5)
        ttk.Button(ctrl_frame, text="Stop", command=lambda: self.send_command("stop")).grid(row=0, column=1, padx=5, pady=5)
        ttk.Button(ctrl_frame, text="Power Off", command=lambda: self.send_command("power")).grid(row=0, column=2, padx=5, pady=5)
        
        # Status frame
        status_frame = ttk.LabelFrame(root, text="Status", padding="5")
        status_frame.grid(row=4, column=0, padx=5, pady=5, sticky="ew")
        
        # Status text
        self.status_text = tk.Text(status_frame, height=5, width=40)
        self.status_text.grid(row=0, column=0, padx=5, pady=5)
        
        # Make window non-resizable
        root.resizable(False, False)
    
    def refresh_ports(self):
        """Refresh the list of available serial ports"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports:
            self.port_combo.set(ports[0])
    def connect_serial(self):
        """Connect or disconnect from the serial port"""
        if self.serial is None:
            try:
                self.serial = serial.Serial(self.port_var.get(), 115200, timeout=1)
                self.serial_btn.configure(text="Disconnect Serial")
                self.status_text.delete(1.0, tk.END)
                self.status_text.insert(tk.END, "Serial port connected\n")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to connect to serial port: {str(e)}")
        else:
            self.serial.close()
            self.serial = None
            self.serial_btn.configure(text="Connect Serial")
            self.status_text.delete(1.0, tk.END)
            self.status_text.insert(tk.END, "Serial port disconnected\n")
    
    def send_command(self, command):
        """Send a command to the ESP32"""
        if self.serial is None:
            messagebox.showwarning("Warning", "Not connected to ESP32")
            return
        
        try:
            self.serial.write(f"{command}\n".encode())
            # Read response
            response = ""
            while True:
                line = self.serial.readline().decode().strip()
                if not line:
                    break
                response += line + "\n"
            
            # Update status text
            self.status_text.delete(1.0, tk.END)
            self.status_text.insert(tk.END, response)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send command: {str(e)}")

if __name__ == "__main__":
    root = tk.Tk()
    app = GoProControl(root)
    root.mainloop()
