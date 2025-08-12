# esp_serial_comm_test



#### Prequisites
- This would run on **Linux (Ubuntu)**, **Windows**, and **MAC OS**

> [!NOTE]  
> For Windows and Mac Users, ensure you have the [**`CH340 serial converter`**](https://sparks.gogo.co.nz/ch340.html?srsltid=AfmBOooJ45evOXTZdp96-_eI1A2xCokPqFyJm0e_Ybx6LOwyY0qJ5Uux) driver installed.
>
> For Ubuntu Users - the **CH340** driver is installed by default.

- Ensure you have `python3` installed on your PC and also `pip`

- install python virtual environment
  > ```shell
  > sudo apt install python3-pip   # linux or mac users
  > sudo apt install python3-virtualenv   # linux or mac users
  > ```
  > *OR*
  > ```shell
  > pip install virtualenv   # windows users (ensure you have pip installed)
  > ```
  
- Ensure you have the **`Easy IMU Module`** connected to the PC.

#

#### Run App First Time [ Ubuntu or Mac Users ]
- change directory into the **py-serial-client** folder
  > ```shell
  > cd py-serial-client/
  > ```

- create a python virtual environment named **`.env`** in the root folder
  > ```shell
  > python3 -m venv .env
  > ```

- activate the virtual environment
  > ```shell
  > source .env/bin/activate
  > ```

- you should see now that you are in the **`.env`** virtual environment

- install all required python modules
  > ```shell
  > pip3 install -r requirements.txt
  > ```

- now you can run the app in the virtual environment
  > ```shell
  > python3 main.py
  > ```
  
- once you are done using the application, just close and deactivate the environment
  ```shell
    deactivate
  ```