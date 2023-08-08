# Advanced DDoS Protection Module for Apache

## Bolstering Resilience with Dynamic Scoring

Defend your online infrastructure against the relentless tide of Distributed Denial-of-Service (DDoS) attacks using our cutting-edge Advanced DDoS Protection Module. This sophisticated solution employs an intelligent scoring system that identifies and neutralizes potential threats in real-time.

### Tailored Scoring Intelligence

Powered by an ingenious scoring system, our module meticulously assesses incoming requests based on a carefully curated set of filters. This intricate process assigns dynamic scores to each request, empowering you to distinguish benign traffic from potentially malicious activity with unparalleled accuracy. These filters can simply be extended as required.

### Reactive Defense Measures

When the cumulative score of incoming requests breaches a user-defined threshold, our module springs into action. It promptly responds to subsequent requests with a HTTP 429 status code, blocking further expensive requests e.g. on php applications.

### Swift, Custom Ban Enforcement

Elevating security measures, our module seamlessly enforces a custom ban command upon surpassing the threshold. This command serves as an active deterrent, swiftly blocking access from the suspected source and bolstering your system's resistance to sustained attacks.

## Granular Filtering Precision

Experience granular control over your filtering strategy with a diverse array of customizable parameters, empowering you to tailor defenses to your unique environment.

- `userAgent`: Filter requests based on user agents, a pivotal strategy to pinpoint potentially malicious clients.
- `domain`: Employ domain-based filters to focus on specific sections of your application.
- `referer`: Evaluate the source of incoming requests with referer-based filters.
- `request`: Scrutinize request content with contextual filtering.
- `method`: Filter requests by their HTTP methods (GET, POST, etc.).
- `statusCode`: Utilize filters based on HTTP response status codes. (work in progress)
- `score`: Assign predetermined scores to requests, fine-tuning the module's assessment.
- `useRegex`: Opt for advanced pattern matching using regular expressions.
- `applyForBots`: Tailor filtering strategies to include or exclude bot traffic.
- `applyForCDN`: Fine-tune filtering policies for Content Delivery Network (CDN) traffic (work in progress).
- `applyForAssets`: Extend filters to safeguard assets like images, scripts, and stylesheets

Take command of your defense strategy by adjusting critical parameters that shape the module's responsiveness and resilience. These parameters empower you to customize the module's behavior according to your operational needs:

- `maxHits`: Define the maximum number of hits / score from a single source. Once this threshold is reached, the module springs into action, enhancing protection against potential DDoS onslaughts.
- `tickDown`: Fine-tune the time interval (in seconds) at which the module re-evaluates scores and gradually reduces them. This dynamic mechanism ensures that legitimate traffic can be smoothly reintegrated while maintaining vigilance against malicious activity.
- `blockTime`: Establish the duration (in seconds) for which a source is blocked after surpassing the hit threshold. This strategic setting provides a brief yet impactful deterrence against sustained attacks.

## Vertical Scalability Through Redis Integration

Incorporating a Redis backend, our solution offers more than robust security—it assures vertical scalability. This infrastructure enables effortless scaling of resources to gracefully handle varying traffic loads while sustaining top-notch protection.

## Configuration via Apache Directives

Effortlessly configure the module's behavior with precision using these Apache directives:

- `AntiDDoSRedisConnectionType`: Select the Redis server's connection type (unix, tcp).
- `AntiDDoSRedisPath`: Specify the path to the Redis Socket (when using unix as protocol).
- `AntiDDoSRedisTimeout`: Define the timeout for Redis server replies (in ms).
- `AntiDDoSRedisPort`: Configure the Redis server's port.
- `AntiDDoSConfig`: Set the path for the primary anti-DDoS configuration.
- `AntiDDoSConfigLocal`: Specify the path for the local anti-DDoS configuration, enabling strategic selective overwrites.

## Installation and Fortified Defense

1. Ensure the presence of "apxs", "hiredis"  and "g++" utilities.
2. Clone the repository: `git clone https://github.com/Fabian123333/mod_antiddos.git`
3. Navigate to the module directory: `cd apache-ddos-defense`
4. Execute installation: `make`
5. Move Plugin to Apache Libdir `mv .libs/mod_antiddos.so /usr/lib/apache2/modules/`
6. Tailor module options in the Apache configuration file for a seamless integration, minimal config for apache2.local:
```
# apache2.conf:
LoadModule antiddos_module    /usr/lib/httpd/modules/mod_antiddos.so
AntiDDoSConfig /etc/apache2/anti-ddos.json
AntiDDoSConfig /etc/apache2/anti-ddos-local.json
```

## Catalyze Contributions and Nurture Excellence

We enthusiastically invite contributors to fortify our DDoS Protection Module. If you identify avenues for improvement or wish to contribute, initiate a dialogue through GitHub. Your expertise propels our relentless pursuit of excellence.

## Licensing for Empowerment

This endeavor aligns with the ethos of the GPL3 License. For comprehensive details, peruse the [License file](LICENSE).
