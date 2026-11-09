# UART TP : auth and replay

## Preparation:

```sh
uv run main.py keygen lab_key.hex
```

## Manipulation

1. List available ports
    ```sh
    uv run main.py ports
    ```

2. Listening to the board (**CHANGE THE PORT**)
    ```
    uv run main.py --port /dev/cu.usbmodem11302 monitor
    ```

3. Send valid command to the board. The firmware must turn on the led for 1s. (**TAKE CARE OF SEQUENCE NUMBER**)
    ```
    uv run main.py --port /dev/cu.usbmodem11302 --key-file lab_key.hex --sequence 10 --command led --duration 1000 --capture capture.json
    ```

4. Alterate the number without calculating the HMAC again
    ```
    uv run main.py tamper capture.json --field sequence --value 1000 --output altered.json
    uv run main.py --port /dev/cu.usbmodem11302 replay altered.json
    ```

5. Check that legitime command can still run
    ```
    uv run main.py --port /dev/cu.usbmodem11302 send --key-file lab_key.hex --sequence 11 --command led --duration 500
    ```

6. Restart the board and replay `capture.json`. Why does it fail?

## Protocol to implement

```text
id|seq|command|value|hmac
```

|Field|Encoding|
|---|---|
|Id|`re.fullmatch(r"[A-Za-z0-9_-]{64}")`
|Seq|uint32|
|Command|`Led` or `Get`|
|Value|uint32 : sleep time for LED, else 0|
|HMAC|HMAC-SHA256, 64 hex char|
