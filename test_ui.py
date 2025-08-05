from PySide6.QtWidgets import (
    QApplication, QWidget, QPushButton, QLabel, QFileDialog, QVBoxLayout,
    QMessageBox, QTextEdit, QSpacerItem, QSizePolicy
)
from PySide6.QtGui import QFont, QIcon
from PySide6.QtCore import Qt
import sys, subprocess, platform


class DetectAnomalLogApp(QWidget):
    def __init__(self, script_path):
        super().__init__()
        self.script_path = script_path
        self.selected_file = ""

        self.setWindowTitle("Detect Anomal Log")
        self.setWindowIcon(QIcon("default_icon.png"))
        self.setGeometry(300, 200, 700, 500)

        self.init_ui()
        self.setStyleSheet("""
            QPushButton {
                padding: 10px;
                border: 1px solid #999;
                border-radius: 8px;
                background-color: #f9f9f9;
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

        self.title_label = QLabel("APw6")
        self.title_label.setFont(QFont("Arial", 24, QFont.Bold))
        self.title_label.setAlignment(Qt.AlignCenter)

        self.status_label = QLabel("Ready")
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setFont(QFont("Arial", 12, QFont.Bold))
        self.status_label.setStyleSheet("color: gray")

        self.select_btn = QPushButton("Select Log File")
        self.select_btn.clicked.connect(self.select_file)

        self.run_btn = QPushButton("Run Detection")
        self.run_btn.clicked.connect(self.run_script)

        self.log_view = QTextEdit()
        self.log_view.setReadOnly(True)
        self.log_view.setFont(QFont("Courier", 10))
        self.log_view.setStyleSheet("background-color: #f4f4f4;")

        layout.addWidget(self.title_label)
        layout.addWidget(self.status_label)
        layout.addSpacerItem(QSpacerItem(0, 10, QSizePolicy.Minimum, QSizePolicy.Fixed))
        layout.addWidget(self.select_btn)
        layout.addWidget(self.run_btn)
        layout.addSpacerItem(QSpacerItem(0, 10, QSizePolicy.Minimum, QSizePolicy.Fixed))
        layout.addWidget(self.log_view)

        self.setLayout(layout)

    def select_file(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Select Log File", "", "All Files (*)")
        if file_path:
            self.selected_file = file_path
            self.status_label.setText("📄 File selected")
            self.status_label.setStyleSheet("color: blue; font-weight: bold;")

    def run_script(self):
        if not self.selected_file:
            QMessageBox.warning(self, "⚠️ Missing File", "Please select a log file.")
            return

        log_path = "debug_output.log"
        self.status_label.setText("⏳ Running...")
        self.status_label.setStyleSheet("color: orange; font-weight: bold;")
        QApplication.processEvents()

        is_windows = platform.system() == "Windows"
        cmd = ["cmd", "/c", self.script_path, self.selected_file] if is_windows else ["bash", self.script_path, self.selected_file]

        try:
            with open(log_path, "w") as log_file:
                subprocess.run(cmd, stdout=log_file, stderr=log_file, check=True)

            with open(log_path, "r") as f:
                self.log_view.setPlainText(f.read())

            self.status_label.setText("✅ Done")
            self.status_label.setStyleSheet("color: green; font-weight: bold;")
            QMessageBox.information(self, "Done", f"Script finished.\nLog: {log_path}")

        except subprocess.CalledProcessError:
            self.status_label.setText("❌ Failed")
            self.status_label.setStyleSheet("color: red; font-weight: bold;")
            QMessageBox.critical(self, "Error", f"Script failed.\nLog: {log_path}")


if __name__ == "__main__":
    script_path = "./detect_anomal_log.sh"
    app = QApplication(sys.argv)
    app.setWindowIcon(QIcon("default_icon.png"))  
    window = DetectAnomalLogApp(script_path)
    window.show()
    sys.exit(app.exec())
