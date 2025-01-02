# Architecture

![SOCVacinnes](https://github.com/user-attachments/assets/a376d762-4be6-470f-84ea-2866520f98a4)

The SOVACCINES project is a C-based application designed in Linux to simulate a distributed system for vaccine acquisition and distribution, involving three key participants: clients, proxies, and servers. Clients represent healthcare and pharmaceutical facilities (e.g., Alvalade Health Center), proxies act as distribution services (e.g., Alliance Healthcare, S.A.), and servers simulate pharmaceutical laboratories producing and supplying vaccines (e.g., Johnson & Johnson, Ltd). The system operates on a request-response architecture, emphasizing process management, shared memory handling, and synchronization mechanisms. Users can create and monitor requests through an interactive menu, with the project following modular design principles to ensure clear component separation and efficient simulation of real-world vaccine distribution workflows.


# Collaborators
Henrique Rosado - 54433
Francisco Amaro - 54440
Ricardo Pedro - 55309

Limitações:
	O programa só funciona se nós destruirmos os semáforos no inicio da criação deles.
