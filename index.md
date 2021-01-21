


## Introduction

Traditional query explanation systems reply primarily on the user’s complaint as the quality measure and make strong assumptions about the error types. For systems which generate explanations by perturbing input values through models, they either use very simple models or assume that user can magically provide models. This is a pain point: data scientists may have domain-specific knowledge, but asking them to provide models before using the system is impractical. 

Reptile is an explanation system that helps user understand and characterize the subset of the dataset that most contributed to the error by iteratively drill down the complaints. Compared to traditional outlier detectors which detect anomaly in raw data, Reptile attempts to resolve user’s complaints by uncovering anomaly ina ggregated statistics. Detection of anomaly in aggregated statis-tics enables Reptile to unearth a wider range of problems. While previous query explanation systems utilize naive interventions orask users to provide models for anomaly detection, Reptile adopts multi-level model to predict group statistics, which leverages the hierarchical structure in the data to address the scarcity of trainingdata and make more accurate estimates. The use of model empowers the user to specify predictive signals that an explanation systemwould otherwise not be aware of. Reptilefurther utilizes factorised representation to support large-scale matrix operations that are at the heart of model training. 

At the minimum, user only needs to provide the dataset and aggregation functions they want to inspect, and Reptile is all set! Reptile will work like an outlier detector to resolve user's complaints. Reptile allows users to provide small pieces of data as domain-specific knowledge, and Reptile will combine these signals to make more meaningful explanations.







![Image](files/images/back_arch.png)


## Demo

We collobarate with Columbia University Financial Instruments Sector Team, who is working on project to design weather index insurance for Ethiopia. FIST collects farmer reported drought severity (from 1 (low) to 10 (high)) at different villagesin different years in Ethiopia through questionnaires. The questionnaires also record farmers’ reason for each reported drought severity. Ethiopia has a geographical hierarchy: Region, District,Village. Farmer reported data have data cleaning problems which FIST hopes to discover. For example, farmers sometimes confuse the years or the events. Sensing data are available for each village and district each year. Given the large volume of questionnaires,FIST hopes to prioritize on the questionnaires that are likely to contain data cleaning errors. We set up a demo page of Reptilecan to help FIST discover data cleaning problems. Click [here](http://35.232.65.156:5000/com) to see the demo.

The source code for model training optimizations is available [here](https://github.com/zachary62/Dynamic-f-tree).
