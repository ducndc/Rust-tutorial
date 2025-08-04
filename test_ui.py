import sys
import subprocess
from PySide6.QtWidgets import (
    QApplication, QWidget, QPushButton, QLabel, QFileDialog,
    QVBoxLayout, QMessageBox, QTextEdit, QHBoxLayout
)
from PySide6.QtCore import Qt
from PySide6.QtGui import QFont
from datetime import datetime


class DetectAnomalLogApp(QWidget):
    def __init__(self, script_path):
        super().__init__()
        self.setWindowTitle("Detect Anomal Log")
        self.setGeometry(300, 300, 700, 400)

        self.script_path = script_path
        self.selected_file = ""

        # --- Title ---
        self.title_label = QLabel("APw6")
        self.title_label.setAlignment(Qt.AlignCenter)
        self.title_label.setFont(QFont("Arial", 20, QFont.Bold))

        # --- File select ---
        self.select_btn = QPushButton("Select Log File")
        self.select_btn.setFixedHeight(40)
        self.select_btn.clicked.connect(self.select_file)

        self.file_label = QLabel("No file selected")
        self.file_label.setAlignment(Qt.AlignCenter)
        self.file_label.setStyleSheet("color: gray;")

        # --- Run button ---
        self.run_btn = QPushButton("Run Detection")
        self.run_btn.setFixedHeight(40)
        self.run_btn.clicked.connect(self.run_script)

        # --- Output view ---
        self.log_view = QTextEdit()
        self.log_view.setReadOnly(True)
        self.log_view.setStyleSheet("background-color: #f4f4f4; font-family: monospace;")

        self.status_label = QLabel("Ready")
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setStyleSheet("color: gray; font-weight: bold; padding: 6px;")
        

        # --- Layout ---
        layout = QVBoxLayout()
        layout.addWidget(self.title_label)
        layout.addWidget(self.status_label)
        layout.addWidget(self.select_btn)
        # layout.addWidget(self.file_label)
        layout.addWidget(self.run_btn)
        layout.addWidget(self.log_view)
        self.setLayout(layout)

    def select_file(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Select Log File", "", "All Files (*)")
        if file_path:
            self.selected_file = file_path
            self.file_label.setText(f"Selected: {file_path}")
            self.file_label.setStyleSheet("color: green;")

    def run_script(self):
        if not self.selected_file:
            QMessageBox.warning(self, "Missing File", "Please select a log file.")
            return

        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        log_path = f"debug_output.log"
        self.status_label.setText("Processing...")  
        self.status_label.setStyleSheet("color: orange; font-weight: bold;")
        QApplication.processEvents()

        try:
            with open(log_path, "w") as log_file:
                subprocess.run(
                    ["bash", self.script_path, self.selected_file],
                    stdout=log_file,
                    stderr=log_file,
                    check=True
                )

            with open(log_path, "r") as f:
                log_content = f.read()
            self.log_view.setPlainText(log_content)

            self.status_label.setText("Done")
            self.status_label.setStyleSheet("color: green; font-weight: bold;")

            QMessageBox.information(self, "Success", f"Script finished successfully.\nLog saved at:\n{log_path}")

        except subprocess.CalledProcessError:
            self.log_view.setPlainText("Script failed. Check the log file for more info.")
            self.status_label.setText("Fail")
            self.status_label.setStyleSheet("color: red; font-weight: bold;")
            QMessageBox.critical(self, "Error", f"Script failed.\nLog saved at:\n{log_path}")


if __name__ == "__main__":
    script_path = "./detect_anomal_log.sh"  # <-- thay đường dẫn file shell của bạn
    app = QApplication(sys.argv)
    window = DetectAnomalLogApp(script_path)
    window.show()
    sys.exit(app.exec())
