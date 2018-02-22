# Sensel Latency Benchmarking

MaxFrameRate = 1kHz

Task conditions

- No user interaction (no touching the sensor)
- Running inside Bela Aux Task:
  - senselReadSensor(handle)
  - senselGetNumAvailableFrames(handle, &num_frames);
  - senselGetFrame(handle, frame)

Test 1 - All data, low detail

- FrameMask = FRAME_CONTENT_PRESSURE_MASK + FRAME_CONTENT_LABELS_MASK + FRAME_CONTENT_CONTACTS_MASK + FRAME_CONTENT_ACCEL_MASK
- ContactMask = CONTACT_MASK_ELLIPSE + CONTACT_MASK_DELTAS + CONTACT_MASK_BOUNDING_BOX + CONTACT_MASK_PEAK
- ScanDetail = LOW
- Latency: ~5ms
- Jitter: ~3ms

Test 2 - All data, medium detail

- FrameMask = FRAME_CONTENT_PRESSURE_MASK + FRAME_CONTENT_LABELS_MASK + FRAME_CONTENT_CONTACTS_MASK + FRAME_CONTENT_ACCEL_MASK
- ContactMask = CONTACT_MASK_ELLIPSE + CONTACT_MASK_DELTAS + CONTACT_MASK_BOUNDING_BOX + CONTACT_MASK_PEAK
- ScanDetail = MEDIUM
- Latency: ~5ms
- Jitter: ~2ms

Test 3 - All data, high detail

- FrameMask = FRAME_CONTENT_PRESSURE_MASK + FRAME_CONTENT_LABELS_MASK + FRAME_CONTENT_CONTACTS_MASK + FRAME_CONTENT_ACCEL_MASK
- ContactMask = CONTACT_MASK_ELLIPSE + CONTACT_MASK_DELTAS + CONTACT_MASK_BOUNDING_BOX + CONTACT_MASK_PEAK
- ScanDetail = HIGH
- Latency: ~18ms
- Jitter: ~2ms

Test 4 - No extra data, high detail

- FrameMask = 0
- ContactMask = 0
- ScanDetail = HIGH
- Latency: ~10ms
- Jitter: ~2ms

Test 5 - No extra data, low detail

- FrameMask = 0
- ContactMask = 0
- ScanDetail = LOW
- Latency: ~1ms
- Jitter: ~1ms

Test 6 - Force data, low detail

- FrameMask = FRAME_CONTENT_PRESSURE_MASK
- ContactMask = 0
- ScanDetail = LOW
- Latency: ~3ms
- Jitter: ~2ms

Test 7 - Contact data, low detail

- FrameMask = FRAME_CONTENT_CONTACTS_MASK
- ContactMask = CONTACT_MASK_ELLIPSE + CONTACT_MASK_DELTAS + CONTACT_MASK_BOUNDING_BOX + CONTACT_MASK_PEAK;
- ScanDetail = LOW
- Latency: ~1ms
- Jitter: ~1ms

Test 8 - Contact data, high detail

- FrameMask = FRAME_CONTENT_CONTACTS_MASK
- ContactMask = CONTACT_MASK_ELLIPSE + CONTACT_MASK_DELTAS + CONTACT_MASK_BOUNDING_BOX + CONTACT_MASK_PEAK;
- ScanDetail = HIGH
- Latency: ~10ms
- Jitter: ~1ms