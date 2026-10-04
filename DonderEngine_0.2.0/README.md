# Donder Engine 0.2.0

C++ ile geliştirilen ve C# oyun scriptlerini hedefleyen 3D oyun motoru.

## Bu sürüm

- Project Manager
- Proje oluşturma
- Proje konumu seçme
- Projeleri listeleme
- Çift tıklayarak proje açma
- Editor penceresi
- Hierarchy
- Inspector
- Assets
- Console
- Play/Stop görsel kontrolleri
- 3D viewport temeli
- Grid
- Basit perspektif küp gösterimi
- Mouse wheel ile viewport zoom
- GameObject
- Transform
- Scene başlangıç nesneleri

## Derleme

Windows + Visual Studio 2022 C++ Desktop Development:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Çıktı:

`build/bin/Release/DonderEngine.exe`

Bu aşamadan sonra:
1. Gerçek OpenGL/Vulkan renderer
2. Mouse kamera
3. Mesh sistemi
4. Material/Shader
5. Entity Component System
6. Scene save/load
7. C# runtime
8. C# hot reload
9. Physics
10. Game build/export
