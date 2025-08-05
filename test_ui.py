from PySide6.QtWidgets import (
    QApplication, QWidget, QPushButton, QLabel, QFileDialog,
    QVBoxLayout, QMessageBox, QTextEdit, QComboBox
)
from PySide6.QtGui import QFont, QIcon
from PySide6.QtCore import Qt
import sys, subprocess, platform


class AnomalyDetectionApp(QWidget):
    def __init__(self):
        super().__init__()
        self.selected_file = ""
        self.device_type = "APw6"

        self.setWindowTitle("Anomaly Log Detection")
        self.setWindowIcon(QIcon("default_icon.png"))
        self.setGeometry(300, 200, 720, 520)

        self.init_ui()
        self.setStyleSheet("""
            QPushButton {
                padding: 10px;
                border: 1px solid #999;
                border-radius: 8px;
                background-color: #ffffff;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #e6f2ff;
            }
            QLabel {
                font-family: 'Arial';
            }
        """)

    def init_ui(self):
        layout = QVBoxLayout()

        # Device selection
        self.device_selector = QComboBox()
        self.device_selector.addItems(["APw6", "ONTw6", "XGSPONw6", "ONTw5", "APw5"])
        self.device_selector.currentTextChanged.connect(self.update_device_type)

        # Title
        self.title_label = QLabel("Anomaly Log Detection")
        self.title_label.setFont(QFont("Arial", 20, QFont.Bold))
        self.title_label.setAlignment(Qt.AlignCenter)

        # Device label
        self.device_label = QLabel(f"Device: {self.device_type}")
        self.device_label.setAlignment(Qt.AlignCenter)
        self.device_label.setStyleSheet("font-weight: bold; color: navy")

        # Status label
        self.status_label = QLabel("⚪ Ready")
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setStyleSheet("color: gray")

        # Select button
        self.select_btn = QPushButton("Select Log File")
        self.select_btn.clicked.connect(self.select_file)

        # Run button
        self.run_btn = QPushButton("Run Detection")
        self.run_btn.clicked.connect(self.run_script)

        # Log output
        self.log_view = QTextEdit()
        self.log_view.setReadOnly(True)
        self.log_view.setFont(QFont("Courier", 10))
        self.log_view.setStyleSheet("background-color: #f7f7f7;")

        # Add widgets
        layout.addWidget(self.device_selector)
        layout.addWidget(self.device_label)
        layout.addWidget(self.status_label)
        layout.addWidget(self.select_btn)
        layout.addWidget(self.run_btn)
        layout.addWidget(self.log_view)

        self.setLayout(layout)

    def update_device_type(self, text):
        self.device_type = text
        self.device_label.setText(f"Device: {text}")
        self.status_label.setText("⚪ Ready")
        self.status_label.setStyleSheet("color: gray")

    def select_file(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Select Log File", "", "All Files (*)")
        if file_path:
            self.selected_file = file_path
            self.status_label.setText("📄 File selected")
            self.status_label.setStyleSheet("color: blue; font-weight: bold;")

    def run_script(self):
        if not self.selected_file:
            QMessageBox.warning(self, "Missing Log File", "Please select a log file first.")
            return

        script_map = {
            "APw6": "./detect_anomal_log.sh",
            "ONTw6": "./detect_anomal_log.sh",
            "XGSPONw6": "./detect_anomal_log.sh",
            "ONTw5": "./detect_anomal_log.sh",
            "APw5": "./detect_anomal_log.sh"
        }
        script_path = script_map.get(self.device_type)
        if not script_path:
            QMessageBox.critical(self, "Error", f"No script found for device: {self.device_type}")
            return

        log_path = f"debug_output_{self.device_type}.log"
        self.status_label.setText("⏳ Running...")
        self.status_label.setStyleSheet("color: orange; font-weight: bold;")
        QApplication.processEvents()

        is_windows = platform.system() == "Windows"
        cmd = ["cmd", "/c", script_path, self.selected_file] if is_windows else ["bash", script_path, self.selected_file]

        try:
            with open(log_path, "w") as log_file:
                subprocess.run(cmd, stdout=log_file, stderr=log_file, check=True)

            with open(log_path, "r") as f:
                self.log_view.setPlainText(f.read())

            self.status_label.setText("✅ Completed")
            self.status_label.setStyleSheet("color: green; font-weight: bold;")
            QMessageBox.information(self, "Success", f"Script finished.\nLog saved to: {log_path}")

        except subprocess.CalledProcessError:
            self.status_label.setText("❌ Failed")
            self.status_label.setStyleSheet("color: red; font-weight: bold;")
            QMessageBox.critical(self, "Execution Error", f"Script execution failed.\nLog: {log_path}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setWindowIcon(QIcon("default_icon.png"))
    window = AnomalyDetectionApp()
    window.show()
    sys.exit(app.exec())