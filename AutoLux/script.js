// Car data array
const cars = [
    { id: "merc-s-class", name: "Mercedes S-Class", price: 89999, category: "luxury", specs: "V8 Engine • 2023 • 15K miles", color: "222222" },
    { id: "porsche-911", name: "Porsche 911", price: 125999, category: "sport", specs: "Turbo • 2024 • 8K miles", color: "CC0000" },
    { id: "range-rover-sport", name: "Range Rover Sport", price: 79999, category: "suv", specs: "Supercharged • 2023 • 22K miles", color: "004D00" },
    { id: "bmw-7-series", name: "BMW 7 Series", price: 72999, category: "luxury", specs: "Hybrid • 2024 • 12K miles", color: "001A4D" },
    { id: "audi-r8", name: "Audi R8", price: 159999, category: "sport", specs: "V10 • 2023 • 6K miles", color: "FFD700" },
    { id: "lamborghini-urus", name: "Lamborghini Urus", price: 219999, category: "suv", specs: "V8 Twin-Turbo • 2024 • 3K miles", color: "330033" },
    { id: "tesla-model-s", name: "Tesla Model S", price: 75000, category: "electric", specs: "Electric • 2023 • 10K miles", color: "228B22" },
    { id: "porsche-taycan", name: "Porsche Taycan", price: 105000, category: "electric", specs: "Electric • 2024 • 5K miles", color: "1E90FF" }, 
    { id: "ferrari-488", name: "Ferrari 488 GTB", price: 250000, category: "supercar", specs: "V8 • 2022 • 2K miles", color: "FF8C00" },
    { id: "ford-mustang-67", name: "1967 Ford Mustang", price: 60000, category: "classic", specs: "V8 • Classic • Restored", color: "8B4513" },
    { id: "bmw-x7", name: "BMW X7", price: 85000, category: "suv", specs: "Inline-6 • 2024 • 18K miles", color: "4169E1" },
    { id: "ford-f150-raptor", name: "Ford F-150 Raptor", price: 78000, category: "truck", specs: "EcoBoost V6 • 2023 • 25K miles", color: "A9A9A9" }
];

// Helper to format currency
function formatCurrency(amount) {
    return `$${amount.toLocaleString(undefined, { minimumFractionDigits: 0, maximumFractionDigits: 0 })}`;
}

// Function to update the login status display throughout the UI
function updateLoginStatusUI() {
    const loggedInEmail = localStorage.getItem('loggedInUser');
    const $currentUserId = $('#currentUserId');
    const $loginLogoutLink = $('#loginLogoutLink');
    const $loginStatus = $('#loginStatus');
    const $loginForm = $('#loginForm');
    const $loginTitle = $('#loginTitle');
    const $emptyCartMessage = $('#emptyCartMessage');

    if (loggedInEmail) {
        $currentUserId.text(loggedInEmail);
        $loginLogoutLink.text('Logout');

        if ($loginStatus.length > 0) {
            $loginStatus.html(`Logged in as: <strong>${loggedInEmail}</strong>`);
            $loginForm.hide();
            $loginTitle.hide();
        }
        if ($emptyCartMessage.length > 0) {
            $emptyCartMessage.html('Your cart is empty. Add some premium cars!');
        }
    } else {
        $currentUserId.text('Not logged in');
        $loginLogoutLink.text('Login');

        if ($loginStatus.length > 0) {
            $loginStatus.html(`Not logged in.`);
            $loginForm.show();
            $loginTitle.show();
        }
        if ($emptyCartMessage.length > 0) {
            $emptyCartMessage.html('Your cart is empty. Please log in to manage your preferences.');
        }
    }
}

// Function to register a new user
function registerUser(email, password) {
    const $loginError = $('#loginError');
    
    // Validate inputs
    if (!email || !email.includes('@')) {
        $loginError.text('Please enter a valid email address.').show();
        return false;
    }
    
    if (!password || password.length < 4) {
        $loginError.text('Password must be at least 4 characters.').show();
        return false;
    }
    
    // Send registration data to PHP
    $.ajax({
        type: 'POST',
        url: 'http://localhost:8000/api.php',
        data: { 
            register: true,
            email: email, 
            password: password 
        },
        success: function(response) {
            try {
                const result = JSON.parse(response);
                if (result.success) {
                    localStorage.setItem('loggedInUser', email);
                    alert('Registration successful!');
                    updateLoginStatusUI();
                    updateUserListUI();
                } else {
                    $loginError.text(result.message).show();
                }
            } catch (e) {
                $loginError.text('Error processing registration. Please try again.').show();
                $('#debugInfo').text('JSON Parse Error: ' + e.message);
            }
        },
        error: function(xhr) {
            $loginError.text('Server error. Please try again.').show();
            $('#debugInfo').text('AJAX Error: ' + xhr.status + ' ' + xhr.statusText);
        }
    });
}

// Function to log in an existing user
function loginUser(email, password) {
    const $loginError = $('#loginError');
    
    // Send login data to PHP
    $.ajax({
        type: 'POST',
        url: 'http://localhost:8000/api.php',
        data: { 
            login: true,
            email: email, 
            password: password 
        },
        success: function(response) {
            try {
                const result = JSON.parse(response);
                if (result.success) {
                    localStorage.setItem('loggedInUser', email);
                    alert('Logged in successfully!');
                    updateLoginStatusUI();
                } else {
                    $loginError.text(result.message).show();
                }
            } catch (e) {
                $loginError.text('Error processing login. Please try again.').show();
                $('#debugInfo').text('JSON Parse Error: ' + e.message);
            }
        },
        error: function(xhr) {
            $loginError.text('Server error. Please try again.').show();
            $('#debugInfo').text('AJAX Error: ' + xhr.status + ' ' + xhr.statusText);
        }
    });
}

// Function to log out the current user
function logoutUser() {
    localStorage.removeItem('loggedInUser');
    localStorage.removeItem('cartItems');
    alert('Logged out successfully! Your cart has been cleared.');
    updateLoginStatusUI();
}

// Function to update cart UI from Local Storage
function updateCartUI() {
    const $cartItemsContainer = $('#cartItemsContainer');
    const $cartTotalPrice = $('#cartTotalPrice');
    const $cartItemCount = $('#cartItemCount');
    const $emptyCartMessage = $('#emptyCartMessage');
    
    const cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
    let total = 0;

    if ($cartItemsContainer.length > 0) {
        $cartItemsContainer.empty(); 

        if (cartItems.length === 0) {
            $emptyCartMessage.show();
            $('#cartSummary').hide();
        } else {
            $emptyCartMessage.hide();
            $('#cartSummary').show();
            cartItems.forEach(item => {
                const itemTotal = item.price * item.quantity;
                total += itemTotal;
                const cartItemHtml = `
                    <div class="cart-item" data-car-id="${item.carId}">
                        <div class="cart-item-thumbnail"></div>
                        <div class="cart-item-info">
                            <span class="item-name">${item.name}</span>
                            <span class="item-price">${formatCurrency(item.price)}</span>
                        </div>
                        <div class="cart-item-quantity">
                            <button class="quantity-minus" aria-label="Decrease quantity" data-car-id="${item.carId}">-</button>
                            <input type="number" value="${item.quantity}" min="1" class="quantity-input" data-car-id="${item.carId}">
                            <button class="quantity-plus" aria-label="Increase quantity" data-car-id="${item.carId}">+</button>
                        </div>
                        <button class="remove-item-btn" data-car-id="${item.carId}">Remove</button>
                    </div>
                `;
                $cartItemsContainer.append(cartItemHtml);
            });
        }

        $cartTotalPrice.text(formatCurrency(total));
    }
    $cartItemCount.text(cartItems.length);
}

// Function to render contact form submissions from localStorage
function renderContactOrders() {
    const $contactOrdersList = $('#contactOrdersList');
    const $noContactOrdersMessage = $('#noContactOrdersMessage');
    const contactOrders = JSON.parse(localStorage.getItem('contactOrders')) || [];

    $contactOrdersList.empty();

    if (contactOrders.length === 0) {
        $noContactOrdersMessage.show();
    } else {
        $noContactOrdersMessage.hide();
        contactOrders.forEach((order, index) => {
            const orderCardHtml = `
                <div class="order-card" data-order-index="${index}">
                    <h4>Inquiry from ${order.fullName}</h4>
                    <p><strong>Email:</strong> ${order.email}</p>
                    <p><strong>Phone:</strong> ${order.phone || 'N/A'}</p>
                    <p><strong>Interested In:</strong> ${order.interest}</p>
                    <p><strong>Budget:</strong> ${formatCurrency(parseInt(order.budget))}</p>
                    <p><strong>Message:</strong> ${order.message.substring(0, 100)}${order.message.length > 100 ? '...' : ''}</p>
                    <p class="text-xs text-gray-500">Submitted: ${new Date(order.submissionTimestamp).toLocaleString()}</p>
                    <div class="order-actions">
                        <button class="download-btn" data-order-index="${index}">Download</button>
                        <button class="delete-btn" data-order-index="${index}">Delete</button>
                    </div>
                </div>
            `;
            $contactOrdersList.append(orderCardHtml);
        });
    }
}

// Function to update the user list UI
function updateUserListUI() {
    $.ajax({
        type: 'POST',
        url: 'http://localhost:8000/api.php',
        data: { get_users: true },
        success: function(users) {
            try {
                const userData = JSON.parse(users);
                const $userList = $('#userList');
                $userList.empty();
                
                if (userData.length === 0) {
                    $userList.append('<p class="no-users-message">No users found</p>');
                } else {
                    userData.forEach(user => {
                        const userHtml = `
                            <div class="user-item">
                                <div>
                                    <div class="user-email">${user.email}</div>
                                    <div class="user-password">Password: ${user.password}</div>
                                    <div class="user-timestamp">Registered: ${user.timestamp}</div>
                                </div>
                            </div>
                        `;
                        $userList.append(userHtml);
                    });
                }
            } catch (e) {
                $('#userList').html('<p class="no-users-message">Error loading users</p>');
                $('#debugInfo').text('JSON Parse Error: ' + e.message);
            }
        },
        error: function(xhr) {
            $('#userList').html('<p class="no-users-message">Error loading users</p>');
            $('#debugInfo').text('User List AJAX Error: ' + xhr.status + ' ' + xhr.statusText);
        }
    });
}

// Function to attach event listeners that need to be bound after initial load
function attachGlobalEventListeners() {
    // Explore Collection button smooth scroll
    $('#exploreBtn').on('click', function() {
        $('html, body').animate({
            scrollTop: $('#our-collection').offset().top - $('.navbar').outerHeight()
        }, 800);
    });

    // Update budget value display
    $('#budget').on('input', function() {
        $('#budgetValue').text('$' + parseInt($(this).val()).toLocaleString());
    });

    // Contact Form submission 
    $('#contactForm').on('submit', function(event) {
        event.preventDefault();

        const formData = {
            fullName: $('#fullName').val(),
            email: $('#email').val(),
            phone: $('#phone').val(),
            interest: $('#interest').val(),
            budget: $('#budget').val(),
            message: $('#message').val(),
            newsletter: $('#newsletter').is(':checked'),
            submissionTimestamp: new Date().toISOString()
        };

        let contactOrders = JSON.parse(localStorage.getItem('contactOrders')) || [];
        contactOrders.push(formData);
        localStorage.setItem('contactOrders', JSON.stringify(contactOrders));

        // Convert to JSON and download
        const jsonData = JSON.stringify(formData, null, 2);
        const blob = new Blob([jsonData], { type: 'application/json' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        const date = new Date();
        const filename = `autolux_inquiry_${formData.fullName.replace(/[^a-z0-9]/gi, '_')}_${date.getFullYear()}-${(date.getMonth() + 1).toString().padStart(2, '0')}-${date.getDate().toString().padStart(2, '0')}_${date.getHours().toString().padStart(2, '0')}-${date.getMinutes().toString().padStart(2, '0')}-${date.getSeconds().toString().padStart(2, '0')}.json`;
        a.download = filename;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);

        alert('Your message has been successfully saved and downloaded! Check "Last Orders" section.');
        $('#contactForm')[0].reset();
        $('#budgetValue').text('$' + $('#budget').val().toLocaleString());
        renderContactOrders();
    });

    // Our Collection Filter functionality
    $('.filter-btn').on('click', function() {
        $('.filter-btn').removeClass('active');
        $(this).addClass('active');

        var filter = $(this).data('filter');

        $('.car-card').each(function() {
            var category = $(this).data('category');
            if (filter === 'all' || filter === category) {
                $(this).removeClass('hidden').slideDown(300);
            } else {
                $(this).slideUp(300, function() {
                    $(this).addClass('hidden');
                });
            }
        });
    });
    $('.filter-btn[data-filter="all"]').click();

    // Login/Register Form Handling
    let isLoginMode = true;

    $('#toggleAuthForm').on('click', function() {
        isLoginMode = !isLoginMode;
        if (isLoginMode) {
            $('#loginTitle').text('Login');
            $('#authSubmitBtn').text('Login');
            $('#toggleAuthForm').text("Don't have an account? Register here.");
        } else {
            $('#loginTitle').text('Register');
            $('#authSubmitBtn').text('Register');
            $('#toggleAuthForm').text("Already have an account? Login here.");
        }
        $('#loginError').hide().text('');
    });

    $('#loginForm').on('submit', function(event) {
        event.preventDefault();
        $('#loginError').hide().text('');

        const email = $('#loginEmail').val();
        const password = $('#loginPassword').val();

        if (!email || !password) {
            $('#loginError').text('Please enter email and password.').show();
            return;
        }

        if (isLoginMode) {
            loginUser(email, password);
        } else {
            registerUser(email, password);
        }
        $('#loginForm')[0].reset();
    });

    // Add to Cart functionality
    $(document).on('click', '.add-to-cart-btn', function() {
        if ($('#loginLogoutLink').text().trim() === 'Login') {
            alert('Please log in to add items to your cart.');
            return;
        }

        const $card = $(this).closest('.car-card');
        const carId = $card.data('id');
        const carName = $card.data('name');
        const carPrice = parseInt($card.data('price'));

        let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
        let existingItem = cartItems.find(item => item.carId === carId);

        if (existingItem) {
            existingItem.quantity += 1;
            alert(`Increased quantity of ${carName} in cart to ${existingItem.quantity}!`);
        } else {
            cartItems.push({ carId: carId, name: carName, price: carPrice, quantity: 1 });
            alert(`${carName} added to cart!`);
        }
        localStorage.setItem('cartItems', JSON.stringify(cartItems));
        updateCartUI();
    });

    // Cart quantity update and remove item functionality
    $(document).on('click', '.quantity-plus', function() {
        const carId = $(this).data('car-id');
        let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
        let item = cartItems.find(i => i.carId === carId);

        if (item) {
            item.quantity += 1;
            localStorage.setItem('cartItems', JSON.stringify(cartItems));
            updateCartUI();
        }
    });

    $(document).on('click', '.quantity-minus', function() {
        const carId = $(this).data('car-id');
        let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
        let item = cartItems.find(i => i.carId === carId);

        if (item) {
            item.quantity -= 1;
            if (item.quantity < 1) {
                 cartItems = cartItems.filter(i => i.carId !== carId);
                 localStorage.setItem('cartItems', JSON.stringify(cartItems));
                 alert('Item removed from cart!');
            } else {
                localStorage.setItem('cartItems', JSON.stringify(cartItems));
            }
            updateCartUI();
        }
    });
    
    $(document).on('change', '.quantity-input', function() {
        const carId = $(this).data('car-id');
        let newQuantity = parseInt($(this).val());
        let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
        let item = cartItems.find(i => i.carId === carId);

        if (item) {
            if (isNaN(newQuantity) || newQuantity < 1) {
                newQuantity = 1;
                $(this).val(newQuantity);
            }
            item.quantity = newQuantity;
            localStorage.setItem('cartItems', JSON.stringify(cartItems));
            updateCartUI();
        }
    });

    $(document).on('click', '.remove-item-btn', function() {
        const carId = $(this).data('car-id');
        let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
        cartItems = cartItems.filter(item => item.carId !== carId);
        localStorage.setItem('cartItems', JSON.stringify(cartItems));
        updateCartUI();
        alert('Item removed from cart!');
    });

    // Checkout button functionality
    $('#checkoutBtn').on('click', function() {
        if ($('#loginLogoutLink').text().trim() === 'Login') {
            alert('Please log in to proceed to checkout.');
            return;
        }

        const currentTotal = parseFloat($('#cartTotalPrice').text().replace('$', '').replace(/,/g, ''));
        if (currentTotal > 0) {
            $('#modalTotalPrice').text(formatCurrency(currentTotal));
            $('#paymentStatusMessage').text('');
            $('#cardNumberInput').val('');
            $('#paymentModalOverlay').addClass('active');
        } else {
            alert('Your cart is empty. Please add items before checking out.');
        }
    });

    // Payment modal "Pay Now" button functionality
    $('#payNowBtn').on('click', function() {
        const cardNumber = $('#cardNumberInput').val().trim();
        if (cardNumber.length < 10) {
            $('#paymentStatusMessage').text('Please enter a valid card number (at least 10 digits).').css('color', '#e74c3c');
            return;
        }

        $('#paymentStatusMessage').text('Processing payment...').css('color', '#f7931e');
        setTimeout(() => {
            localStorage.removeItem('cartItems');
            updateCartUI();
            $('#paymentStatusMessage').text('Payment successful! Your order has been placed.').css('color', '#28a745');
            
            setTimeout(() => {
                $('#paymentModalOverlay').removeClass('active');
            }, 2000); 
        }, 1500);
    });

    // Payment modal "Close" button functionality
    $('#closePaymentModal').on('click', function() {
        $('#paymentModalOverlay').removeClass('active');
    });

    // Download individual contact orders
    $(document).on('click', '.order-card .download-btn', function() {
        const index = $(this).data('order-index');
        const contactOrders = JSON.parse(localStorage.getItem('contactOrders')) || [];
        const orderData = contactOrders[index];

        if (orderData) {
            const jsonData = JSON.stringify(orderData, null, 2);
            const blob = new Blob([jsonData], { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            const filename = `autolux_inquiry_${orderData.fullName.replace(/[^a-z0-9]/gi, '_')}_${new Date(orderData.submissionTimestamp).toISOString().replace(/[:.]/g, '-')}.json`;
            a.download = filename;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);
            alert(`Inquiry from ${orderData.fullName} downloaded!`);
        } else {
            alert('Error: Could not find order data to download.');
        }
    });

    // Delete individual contact orders
    $(document).on('click', '.order-card .delete-btn', function() {
        const indexToDelete = $(this).data('order-index');
        let contactOrders = JSON.parse(localStorage.getItem('contactOrders')) || [];
        alert(`Inquiry from ${contactOrders[indexToDelete].fullName} will be deleted.`);
        contactOrders.splice(indexToDelete, 1);
        localStorage.setItem('contactOrders', JSON.stringify(contactOrders));
        renderContactOrders();
    });
}

// Generate car cards in the collection section 
function generateCarCards() {
    const $carGrid = $('.car-grid');
    $carGrid.empty();
    
    cars.forEach(car => {
        const carCard = `
            <div class="car-card" data-category="${car.category}" data-id="${car.id}" data-name="${car.name}" data-price="${car.price}">
                <div class="car-image" style="background: url('https://placehold.co/400x200/${car.color.replace('#', '')}/FFF?text=${encodeURIComponent(car.name)}') no-repeat center center; background-size: cover;"></div>
                <div class="car-info">
                    <h3>${car.name}</h3>
                    <p class="price">${formatCurrency(car.price)}</p>
                    <p class="specs">${car.specs}</p>
                    <button class="car-btn add-to-cart-btn">Add to Cart</button>
                </div>
            </div>
        `;
        $carGrid.append(carCard);
    });
}

$(document).ready(function() {
    // Set current year in footer
    $('#currentYear').text(new Date().getFullYear());

    // Generate car cards 
    generateCarCards();

    // Smooth scrolling for navigation links
    $('.nav-link').on('click', function(event) {
        if (this.hash !== "") {
            event.preventDefault();
            var hash = this.hash;
            $('html, body').animate({
                scrollTop: $(hash).offset().top - $('.navbar').outerHeight()
            }, 800, function(){
                window.location.hash = hash;
            });
            $('#nav-menu').removeClass('active');
            $('#hamburger').removeClass('active');
        }
    });

    // Hamburger menu toggle
    $('#hamburger').on('click', function() {
        $('#nav-menu').toggleClass('active');
        $(this).toggleClass('active');
    });

    // Attach all global event listeners
    attachGlobalEventListeners();

    // Initial UI updates
    updateLoginStatusUI();
    updateCartUI();
    renderContactOrders();
    updateUserListUI();

    // Handle Logout link
    $('#loginLogoutLink').on('click', function(e) {
        if ($(this).text() === 'Logout') {
            e.preventDefault();
            logoutUser();
            updateCartUI();
        }
    });
});