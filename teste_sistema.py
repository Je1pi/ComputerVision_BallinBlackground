#!/usr/bin/env python3
"""
Script de teste para verificar dependências do projeto Ball-and-Plate
Execute este script antes de usar o sistema principal
"""

import sys

def test_imports():
    """Testa se todas as dependências estão instaladas"""
    dependencies = []
    
    # Teste OpenCV
    try:
        import cv2
        dependencies.append(("OpenCV", cv2.__version__, "OK"))
    except ImportError:
        dependencies.append(("OpenCV", "Não instalado", "ERRO"))
    
    # Teste NumPy
    try:
        import numpy as np
        dependencies.append(("NumPy", np.__version__, "OK"))
    except ImportError:
        dependencies.append(("NumPy", "Não instalado", "ERRO"))
    
    # Teste PySerial
    try:
        import serial
        dependencies.append(("PySerial", serial.__version__, "OK"))
    except ImportError:
        dependencies.append(("PySerial", "Não instalado", "ERRO"))
    
    return dependencies

def test_camera():
    """Testa se a câmera está funcionando"""
    try:
        import cv2
        cap = cv2.VideoCapture(0)
        if cap.isOpened():
            ret, frame = cap.read()
            cap.release()
            if ret:
                return ("Câmera", "Funcionando", "OK")
            else:
                return ("Câmera", "Erro na captura", "ERRO")
        else:
            return ("Câmera", "Não disponível", "ERRO")
    except Exception as e:
        return ("Câmera", f"Erro: {e}", "ERRO")

def test_serial_ports():
    """Lista portas seriais disponíveis"""
    try:
        import serial.tools.list_ports
        ports = list(serial.tools.list_ports.comports())
        if ports:
            return [(port.device, port.description, "DISPONIVEL") for port in ports]
        else:
            return [("Nenhuma porta", "Conecte o Arduino", "ERRO")]
    except Exception as e:
        return [("Erro", f"{e}", "ERRO")]

def main():
    print("Teste de Dependencias - Ball-and-Plate System")
    print("=" * 50)
    
    # Teste de dependências Python
    print("\nDependencias Python:")
    dependencies = test_imports()
    for name, version, status in dependencies:
        print(f"[{status}] {name}: {version}")
    
    # Teste da câmera
    print("\nTeste da Camera:")
    camera_status = test_camera()
    print(f"[{camera_status[2]}] {camera_status[0]}: {camera_status[1]}")
    
    # Teste das portas seriais
    print("\nPortas Seriais Disponiveis:")
    ports = test_serial_ports()
    for device, description, status in ports:
        print(f"[{status}] {device}: {description}")
    
    # Verificar se tudo está OK
    all_deps_ok = all(dep[2] == "OK" for dep in dependencies)
    camera_ok = camera_status[2] == "OK"
    ports_available = any(port[2] == "DISPONIVEL" for port in ports)
    
    print("\n" + "=" * 50)
    if all_deps_ok and camera_ok and ports_available:
        print("Sistema pronto! Todos os componentes estao funcionando.")
        print("\nProximos passos:")
        print("1. Conecte o Arduino via USB")
        print("2. Carregue o codigo Arduino/ball-and-plate.ino")
        print("3. Execute: python calibrador_hsv.py")
        print("4. Execute: python webcam_teste.py")
    else:
        print("Alguns problemas foram detectados:")
        if not all_deps_ok:
            print("  - Instale as dependencias:")
            print("    pip install -r requirements.txt")
            print("    ou manualmente: pip install opencv-python pyserial numpy")
        if not camera_ok:
            print("  - Verifique se a webcam esta conectada e funcionando")
        if not ports_available:
            print("  - Conecte o Arduino via USB")

if __name__ == "__main__":
    main()
