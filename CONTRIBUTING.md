# Guía de contribución y entrega

Este documento define el protocolo estricto para desarrollar y entregar el **Proyecto 1: ula-malloc**. Sigue estos pasos al pie de la letra para asegurar que tu entrega sea válida.

## 1. Configuración del repositorio (Setup)

No trabajes directamente sobre el repositorio principal de la materia.

1. **Fork:** Haz un "Fork" de este repositorio hacia tu cuenta personal de GitHub (botón arriba a la derecha).
2. **Clone:** Clona **tu** repositorio (el fork) en tu máquina local o entorno WSL.

```bash
git clone https://github.com/TU_USUARIO/os-project-malloc.git
cd os-project-malloc

```

3. **Privacidad:** Si GitHub lo permite, mantén tu repositorio público. Si usas GitLab o Bitbucket por tu cuenta, asegúrate de dar acceso al profesor, pero la entrega final **debe** ser un Pull Request en GitHub.

## 2. Flujo de trabajo (Git Flow)

Para mantener el orden y demostrar profesionalismo:

1. **Crea una Rama (Branch):** No trabajes en la rama `main`. Crea una rama de desarrollo.
```bash
git checkout -b dev/implementacion

```


2. **Commits atómicos:** Haz commits frecuentes con mensajes claros.
* ❌ Mal: "Arreglos varios"
* ✅ Bien: "Implementada estructura block_meta" o "Corregido bug en fusión de bloques (coalescing)"


3. **Ignora binarios:** El archivo `.gitignore` ya está configurado. **Nunca** subas archivos `.o`, `.exe` o el binario `test_runner`. Solo sube código fuente (`.c`, `.h`).

## 3. Normas de código

* **Lenguaje:** C Estándar (C11 o C99).
* **Prohibiciones:**
* Está terminantemente prohibido incluir `<stdlib.h>` para gestión de memoria.
* Si el linter detecta el uso de `malloc`, `calloc`, `realloc` o `free` del sistema, la nota es **0 inmediata**.


* **Variables globales:** Solo se permite una variable global: el puntero al inicio de la lista enlazada (heap base). El resto debe manejarse con paso de parámetros.
* **Comentarios:** Comenta las partes complejas de tu aritmética de punteros. Si no se entiende y no lo puedes explicar, no funciona.

## 4. Proceso de entrega (Pull Request)

La entrega se formaliza abriendo un **Pull Request (PR)** desde tu repositorio hacia el repositorio original de la cátedra.

1. Asegúrate de que tu código compila limpiamente:
```bash
make clean
make

```


2. Sube tus cambios a tu repositorio:
```bash
git push origin dev/implementacion

```


3. Ve a GitHub y abre el Pull Request comparando tu rama con `main`.
4. **Título del PR:** `Proyecto 1 - [Nombre Apellido] - [Cédula]`
5. **Descripción del PR:** Copia y completa esta plantilla obligatoria:

```markdown
### Información del Estudiante
* **Nombre:** Juan Pérez
* **Cédula:** V-12.345.678

### Detalles Técnicos
* **Algoritmo de Asignación:** (First-Fit / Best-Fit)

### Declaración de Uso de IA (Transparencia)
* **¿Usaste IA (ChatGPT, Claude, Copilot)?** (Sí / No)
* **Prompts utilizados:**
  > (Copia y pega aquí lo que le preguntaste a la IA, ej: "¿Cómo alineo un puntero a 8 bytes?")
* **Código generado/adaptado:**
  > (Indica qué función o bloque lógico proviene de la IA)
* **Explicación:**
  > (Explica con tus palabras cómo funciona el código que te dio la IA. Si no puedes explicarlo, no lo uses).

### Comentarios Adicionales
(Explica aquí cualquier decisión de diseño importante o si alguna funcionalidad quedó incompleta)

```

## 5. Integridad Académica y Política de IA

En esta cátedra, **la IA es una herramienta, no un sustituto del ingeniero**.

* **Transparencia radical:** Tienes permitido usar herramientas como ChatGPT o GitHub Copilot para depurar, entender errores o sugerir algoritmos, **SIEMPRE Y CUANDO** lo documentes explícitamente en la sección "Declaración de Uso de IA" del Pull Request.
* **La Regla de la Defensa:**
  * Si usas código generado por IA y lo documentas: **Válido**.
  * Si usas código generado por IA y **NO** lo documentas: **Plagio (Nota: 0)**.
  * Si documentas el código pero en la defensa oral **no puedes explicar cómo funciona** (porque solo hiciste Copy-Paste): **Reprobado**.



Tu capacidad para explicar el código (sea tuyo o de la IA) determina el 100% de tu nota individual.
