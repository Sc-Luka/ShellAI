<?php

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $usersFile = 'users.txt';

    // Create file if it doesn't exist
    if (!file_exists($usersFile)) {
        file_put_contents($usersFile, '');
        chmod($usersFile, 0666); 
    }

    // registration
    if (isset($_POST['register'])) {
        $email = trim($_POST['email'] ?? '');
        $password = trim($_POST['password'] ?? '');

        if (empty($email) || empty($password)) {
            echo json_encode(['success' => false, 'message' => 'Email and password are required.']);
            exit;
        }

        $users = file($usersFile, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        foreach ($users as $user) {
            list($storedEmail) = explode(':', $user);
            if ($storedEmail === $email) {
                echo json_encode(['success' => false, 'message' => 'Email already registered.']);
                exit;
            }
        }

        $userData = "$email:$password:" . date('Y-m-d H:i:s') . PHP_EOL;
        if (file_put_contents($usersFile, $userData, FILE_APPEND | LOCK_EX) === false) {
            echo json_encode(['success' => false, 'message' => 'Failed to save user data.']);
        } else {
            echo json_encode(['success' => true, 'message' => 'Registration successful!']);
        }
        exit;
    }

    // Handle login
    if (isset($_POST['login'])) {
        $email = trim($_POST['email'] ?? '');
        $password = trim($_POST['password'] ?? '');

        if (empty($email) || empty($password)) {
            echo json_encode(['success' => false, 'message' => 'Email and password are required.']);
            exit;
        }

        $users = file($usersFile, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        foreach ($users as $user) {
            $parts = explode(':', $user);
            if (count($parts) >= 2 && $parts[0] === $email && $parts[1] === $password) {
                echo json_encode(['success' => true, 'message' => 'Login successful!']);
                exit;
            }
        }

        echo json_encode(['success' => false, 'message' => 'Invalid email or password.']);
        exit;
    }

    // get_users
    if (isset($_POST['get_users'])) {
        $users = [];
        $lines = file($usersFile, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        foreach ($lines as $line) {
            $parts = explode(':', $line);
            if (count($parts) >= 3) {
                $users[] = [
                    'email' => $parts[0],
                    'password' => $parts[1], 
                    'timestamp' => $parts[2]
                ];
            }
        }
        echo json_encode($users);
        exit;
    }
}
?>
