This project was modeled using MS Directx12 sample code from D3D12nBodyGravity. It simply creates a Christmas Tree made from 1024 point sprites in 32 rotating rings.
Each point sprite is controlled by a GPU thread. The app starts by randomly distributing the points in space, computes the direction vector for each one to its 
location on the tree, and moves there at the same pace. Once the tree is complete, the camera will move towards the center of the tree after 30 seconds.

This is my first DirectX12 simulation. Used Direct 9 many many years ago so learning 12 has been quite the challenge! Looking forward to learning more and writing some more simple simulations and games. 

![image](https://github.com/user-attachments/assets/5200be1f-e253-4457-9fba-b5c70c9bef2d)


